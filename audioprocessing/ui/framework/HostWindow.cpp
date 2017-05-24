#include "pch.h"

#include <Windowsx.h>
//#include <SFML/Window/Event.hpp>

#include <ui/framework/HostWindow.h>
#include <ui/framework/Canvas.h>
#include <ui/framework/Control.h>
#include "Environment.h"

namespace {
sf::VideoMode videomodeFrom(Control * control)
{
    IntSize size = control->size();
    return sf::VideoMode(size.width(), size.height());
}
}   // namespace

HostWindow::HostWindow(const std::string & title, Control * content,
                       const sf::ContextSettings & settings)
    : base(videomodeFrom(content), title, sf::Style::Default, settings)
    , content_(content)
    , baseWndProc_(nullptr)
{
    // remember current size as minimal size of window
    RECT r;
    GetWindowRect(getSystemHandle(), &r);
    minSize_ = { r.right - r.left, r.bottom - r.top };

    content_->setHostWindow(this);
}

HostWindow::~HostWindow()
{}

void HostWindow::storeCurrentSize()
{
    RECT rect;
    GetClientRect(getSystemHandle(), &rect);
    size_ = UIntSize{ unsigned(rect.right - rect.left), unsigned(rect.bottom - rect.top) };
}

namespace {

ATOM s_wndSubclass = GlobalAddAtom("MUSICFX");

void setTracking(HWND hWnd, bool track)
{
    TRACKMOUSEEVENT mouseEvent;
    mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    mouseEvent.dwFlags = track ? TME_LEAVE : TME_CANCEL;
    mouseEvent.hwndTrack = hWnd;
    mouseEvent.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&mouseEvent);
}

Point positionFromLParam(LPARAM lParam)
{
    return{ float(GET_X_LPARAM(lParam)),
            float(GET_Y_LPARAM(lParam)) };
}
}   // namespace

Control * HostWindow::activeControl(Point & pt)
{
    Control * activeCtl = pointerCaptureCtl_;

    if(!activeCtl)
        activeCtl = pointerHoverCtl_;

    if(!activeCtl) {
        assert(false);
        return content_;
    }

    Control * walker = activeCtl;

    while(walker != content_) {
        pt -= walker->position();
        walker = walker->parent();
    }

    return activeCtl;
}

namespace {
bool checkModifier(VirtualKeyModifiers actual, VirtualKeyModifiers checked)
{
    return (actual & checked) != VirtualKeyModifiers::None;
}
}

void HostWindow::processPointerPressed(VirtualKeyModifiers vk, LPARAM lParam)
{
    PointerEventArgs args(positionFromLParam(lParam), vk);
    Control * activeCtl = activeControl(args.position_);
    activeCtl->onPointerPressed(args);

    if(checkModifier(vk, VirtualKeyModifiers::LeftButton)) {

        if(DragSource * dragSource = activeCtl->dragSource()) {
            if(dragSource->canDrag()) {
                dragCandidate_ = activeCtl;
                dragClickCoords_ = args.position();
            }
        }

        Control * const lastLClick = lastLClickControl_;
        lastLClickControl_ = activeCtl;

        uint64_t lastClickTime = lastClickTime_;
        uint64_t currentTime = clock_.getElapsedTime().asMilliseconds();
        lastClickTime_ = currentTime;

        if(lastLClick == activeCtl) {
            unsigned dblClickTime = GetDoubleClickTime();

            if(lastClickTime + dblClickTime >= currentTime && lastClickTime_ != -1) {
                activeCtl->onDoubleTapped(args);
                lastClickTime_ = -1;
            }
        }
    }
}

void HostWindow::processPointerReleased(VirtualKeyModifiers vk, LPARAM lParam)
{
    if(checkModifier(vk, VirtualKeyModifiers::LeftButton)) {
        if(DragDrop::isDragging()) {
            PointerEventArgs args(positionFromLParam(lParam), vk);
            Control * hoverControl = content_->getControlAtPoint(args.position_);
            DragDrop::drop(hoverControl, args);
            setHoverControl(hoverControl);
            setPointerCursor(hoverControl->getCursor());
            hoverControl->onPointerMoved(args);
            return;
        }

        dragCandidate_ = nullptr;
    }

    PointerEventArgs args(positionFromLParam(lParam), vk);
    Control * activeCtl = activeControl(args.position_);
    activeCtl->onPointerReleased(args);
}

namespace {
bool shouldStartDrag(Point clickCoord, Point currentCoord)
{
    float dx = clickCoord.x - currentCoord.x;
    float dy = clickCoord.y - currentCoord.y;
    float threshold = float(Environment::dragThreshold());

    return threshold * threshold <= dx * dx + dy * dy;
}
}

void HostWindow::processPointerDragMove(Control * hoverControl, PointerEventArgs & e)
{
    DropEffect dropEffect = DragDrop::processPointerMove(hoverControl, e);

    if(dropEffect == DropEffect::None)
        setPointerCursor(Cursor::No);

	switch (dropEffect)
	{
	case DropEffect::Copy:
		setPointerCursor(Cursor::SizeAll);
		break;

	case DropEffect::Move:
		setPointerCursor(Cursor::Arrow);
		break;

	case DropEffect::Link:
		setPointerCursor(Cursor::UpArrow);
		break;

	default:
		setPointerCursor(Cursor::No);
		break;
	}
}

void HostWindow::setHoverControl(Control * hoverControl)
{
    if (hoverControl != pointerHoverCtl_) {
        if (pointerHoverCtl_)
            pointerHoverCtl_->onPointerExited();

        pointerHoverCtl_ = hoverControl;
        hoverControl->onPointerEntered();
        setPointerCursor(hoverControl->getCursor());
    }
}

void HostWindow::processPointerMove(PointerEventArgs & e)
{
    Control * hoverControl = content_->getControlAtPoint(e.position_);

    if(DragDrop::isDragging()) {
        processPointerDragMove(hoverControl, e);
        return;
    }

    if(hoverControl == dragCandidate_) {
        if(shouldStartDrag(dragClickCoords_, e.position())) {
            dragCandidate_ = nullptr;

            if(DragSource * dragSource = hoverControl->dragSource()) {
                if(dragSource->canDrag()) {
                    PointerEventArgs eventArg(dragClickCoords_, e.keyModifiers());
                    DragDrop::beginDrag(dragSource, hoverControl, eventArg);

                    if(DragDrop::isDragging()) {
                        processPointerDragMove(hoverControl, e);
                        return;
                    }
                }
            }
        }
    }

    setHoverControl(hoverControl);
    hoverControl->onPointerMoved(e);
}

sf::Vector2u HostWindow::getSize() const
{
    return size_;
}

void HostWindow::setPointerCursor(Cursor cursor)
{
    if(cursor == Cursor::Default)
        cursor = Cursor::Arrow;

    if(pointerCursor_ != cursor) {
        pointerCursor_ = cursor;
        SetCursor(CursorManager::getCursorHandle(cursor));
    }
}

void HostWindow::setPointerCapture(Control * ctl)
{
    SetCapture(getSystemHandle());

    if(pointerCaptureCtl_ != ctl) {
        if(pointerCaptureCtl_)
            pointerCaptureCtl_->onPointerCaptureLost();

        pointerCaptureCtl_ = ctl;
    }
}

void HostWindow::releasePointerCapture(Control * ctl)
{
    assert(ctl);

    if(ctl == pointerCaptureCtl_) {
        pointerCaptureCtl_ = nullptr;

        if(GetCapture() == getSystemHandle())
            ReleaseCapture();

        ctl->onPointerCaptureLost();
    }
}

void HostWindow::onResize()
{
    storeCurrentSize();
    needRedraw_ = true;
    redraw();

    // SFML has bug here
#if 0
    base::onResize();
#endif
}

void HostWindow::redraw()
{
    if(!needRedraw_)
        return;

    needRedraw_ = false;

    Rect visibleArea({ 0, 0 }, size_);
    setView(sf::View(visibleArea));

    sf::Time elapsedTime = clock_.getElapsedTime();

    Canvas canvas(this);
    content_->setSize(size_);

    clear(Color::Black);
    canvas.transform().translate(0.5f, 0.5f);
    content_->draw(canvas, elapsedTime);
    display();
}

LRESULT HostWindow::wndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    HostWindow * me = (HostWindow *)GetPropW(handle, (LPCWSTR)s_wndSubclass);
    assert(me);

    switch(message) {
    case WM_SIZE: {
        me->needRedraw_ = true;

        switch(wParam) {
        case SIZE_MAXIMIZED:
        case SIZE_RESTORED: {
            LRESULT result = me->baseWndProc_(handle, message, wParam, lParam);
            me->onResize();
            return result;
        }
        }

        break;

        case WM_GETMINMAXINFO:
            if(lParam) {
                MINMAXINFO * info = reinterpret_cast<MINMAXINFO *>(lParam);
                info->ptMinTrackSize = me->minSize_;
                return 0;
            }
        }

    case WM_SETCURSOR:
        if(LOWORD(lParam) == HTCLIENT) {
            // will set cursor in WM_MOUSEMOVE
            return TRUE;
        }

        break;

    case WM_LBUTTONDOWN:
        me->processPointerPressed(VirtualKeyModifiers::LeftButton, lParam);
        //DefWindowProcW(handle, message, wParam, lParam);
        return 0;

    case WM_LBUTTONUP:
        me->processPointerReleased(VirtualKeyModifiers::LeftButton, lParam);
        //DefWindowProcW(handle, message, wParam, lParam);
        return 0;

    case WM_RBUTTONDOWN:
        me->processPointerPressed(VirtualKeyModifiers::RightButton, lParam);
        //DefWindowProcW(handle, message, wParam, lParam);
        return 0;

    case WM_RBUTTONUP:
        me->processPointerReleased(VirtualKeyModifiers::RightButton, lParam);
        //DefWindowProcW(handle, message, wParam, lParam);
        return 0;

    case WM_MBUTTONDOWN:
        me->processPointerPressed(VirtualKeyModifiers::MiddleButton, lParam);
        return 0;

    case WM_MBUTTONUP:
        me->processPointerReleased(VirtualKeyModifiers::MiddleButton, lParam);
        return 0;

    case WM_XBUTTONDOWN:
        me->processPointerPressed(HIWORD(wParam) == XBUTTON1
                                  ? VirtualKeyModifiers::XButton1
                                  : VirtualKeyModifiers::XButton2, lParam);
        return 0;

    case WM_XBUTTONUP:
        me->processPointerReleased(HIWORD(wParam) == XBUTTON1
                                   ? VirtualKeyModifiers::XButton1
                                   : VirtualKeyModifiers::XButton2, lParam);
        return 0;

    //case WM_LBUTTONDBLCLK: {
    //    PointerEventArgs args(positionFromLParam(lParam), VirtualKeyModifiers(wParam));
    //    Control * activeCtl = me->activeControl(args.position_);
    //    activeCtl->onPointerPressed(args);
    //    return 0;
    //}

    case WM_MOUSELEAVE:
        DefWindowProcW(handle, message, wParam, lParam);

        {
            // Avoid this firing a second time in case the cursor is dragged outside
            if(me->mouseInside_) {
                me->mouseInside_ = false;
                me->content_->onPointerExited();
                me->pointerCursor_ = Cursor(-1);

                if(Control * hoverControl = me->pointerHoverCtl_) {
                    me->pointerHoverCtl_ = nullptr;
                    hoverControl->onPointerExited();
                }
            }

            return 0;
        }

    case WM_MOUSEMOVE:
        DefWindowProcW(handle, message, wParam, lParam);
        {
            float x = short(LOWORD(lParam));
            float y = short(HIWORD(lParam));

            PointerEventArgs args({x, y}, VirtualKeyModifiers(wParam));

            if(me->pointerCaptureCtl_) {
                me->pointerCaptureCtl_->hostToClient(args.position_);
                me->pointerCaptureCtl_->onPointerMoved(args);
            }
            else {

                // Get the client area of the window
                UIntSize area = me->size_;

                //if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
                //{
                //    // Only release the capture if we really have it
                //    if(mouseCaptureHolder_)
                //    if (GetCapture() == m_handle)
                //        ReleaseCapture();
                //}
                //else if (GetCapture() != m_handle)
                //{
                //    // Set the capture to continue receiving mouse events
                //    SetCapture(m_handle);
                //}

                // If the cursor is outside the client area...
                if(x < 0 || x > area.width() || y < 0 || y > area.height()) {
                    // and it used to be inside, the mouse left it.
                    // WTF??? HOW??? ))) D.V.
                    if(me->mouseInside_) {
                        assert(false);
                        me->mouseInside_ = false;
                        setTracking(me->getSystemHandle(), false);
                        me->content_->onPointerExited();
                        me->pointerCursor_ = Cursor(-1);
                        return 0;
                    }
                }
                else {
                    if(!me->mouseInside_) {
                        me->mouseInside_ = true;
                        setTracking(me->getSystemHandle(), true);
                        me->content_->onPointerEntered();
                    }
                }

                me->processPointerMove(args);
            }

            return 0;
        }

    case WM_CAPTURECHANGED:
        DefWindowProcW(handle, message, wParam, lParam);

        if(Control * ctl = me->pointerCaptureCtl_)
            me->releasePointerCapture(ctl);

        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        me->redraw();
        break;
    }

    return me->baseWndProc_(handle, message, wParam, lParam);
}

constexpr int maxRefreshRate = 30;
constexpr sf::Int64 period = 1000000 / maxRefreshRate;    // in microseconds

void HostWindow::run()
{
    storeCurrentSize();

    if(!baseWndProc_) {
        SetPropW(getSystemHandle(), (LPCWSTR)s_wndSubclass, this);

        baseWndProc_ = (WNDPROC)SetWindowLongPtrW(getSystemHandle(),
                                                  GWLP_WNDPROC,
                                                  LONG_PTR(wndProc));
    }

    redraw();

    sf::Event event;

    while(isOpen()) {
        while(pollEvent(event)) {
            switch(event.type) {
            case sf::Event::Closed:
                close();
                return;

            }
        }

        //needRedraw_ = true;

        if(needRedraw_)
            redraw();
        else
            SleepEx(1, TRUE);
    }
}


