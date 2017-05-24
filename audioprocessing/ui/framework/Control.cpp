#include "pch.h"
#include "Control.h"
#include "HostWindow.h"

Control::Control(Rect bounds)
    : position_(bounds.position())
    , size_(bounds.size())
{}

Control::~Control()
{}

void Control::addChildInternal(Control * child)
{
    assert(child);

    if(Control * oldParent = child->parent())
        oldParent->removeChildInternal(child);

    assert(child->parent_ == nullptr);
    assert(child->next_ == nullptr);

    child->parent_ = this;
    child->next_ = firstChild_;
    firstChild_ = child;

    child->setHostWindow(hostWindow_);
}

void Control::removeChildInternal(Control * child)
{
    assert(child);
    assert(child->parent() == this);

    Control ** walker = &firstChild_;

    while(*walker != child)
        walker = &(*walker)->next_;

    *walker = child->next_;
    child->next_ = nullptr;
    child->parent_ = nullptr;
}

// set window to control and all children
void Control::setHostWindow(HostWindow * newWindow)
{
    if(hostWindow_ != newWindow) {
        hostWindow_ = newWindow;

        Control * walker = firstChild_;

        while(walker) {
            walker->setHostWindow(newWindow);
            walker = walker->next_;
        }
    }
}

void Control::setSize(Size newsSize)
{
    if(size_ != newsSize) {
        size_ = newsSize;
        onResized();
    }
}

void Control::setMinSize(Size minSize)
{
    minSize_ = minSize;
}

void Control::onResized() {}

void Control::redraw()
{
    if(HostWindow * wnd = hostWindow_)
        wnd->needRedraw_ = true;
}

void Control::hostToClient(Point & pt)
{
    Control * walker = this;

    while(walker->parent()) {
        pt -= walker->position();
        walker = walker->parent();
    }
}

void Control::setPointerCapture()
{
    hostWindow_->setPointerCapture(this);
}

void Control::releasePointerCapture()
{
    hostWindow_->releasePointerCapture(this);
}

Control * Control::getPointerCapture() const
{
    return hostWindow_->getPointerCapture();
}

Control * Control::getControlAtPoint(Point & pt)
{
    Control * walker = firstChild_;

    while(walker) {
        if(walker->enabled() && walker->bounds().contains(pt)) {
            pt -= walker->position();
            return walker->getControlAtPoint(pt);
        }

        walker = walker->next_;
    }

    return this;
}

void Control::setEnabled(bool value)
{
    if(enabled() != value) {
        enabled_ = value;
        // TODO: update cursor if pointer is under this control
        redraw();
    }
}

Cursor Control::getCursor() const
{
    if(Control * p = parent()) {
        const Cursor cursor = p->getCursor();

        if(cursor != Cursor::Default)
            return cursor;
    }

    return cursor_;
}

void Control::setCursor(Cursor value)
{
    cursor_ = value;
    // TODO: update cursor if pointer is under this control
}

void Control::onPointerEntered()
{
}

void Control::onPointerMoved(PointerEventArgs & e)
{
}

void Control::onPointerExited()
{
}

void Control::onPointerPressed(PointerEventArgs & e) {}

void Control::onPointerReleased(PointerEventArgs & e) {}

void Control::onDoubleTapped(PointerEventArgs & e) {}

void Control::onPointerCaptureLost() {}

void Control::onPointerCanceled(PointerEventArgs & e) {}

void Control::onPointerWheelChanged(PointerEventArgs & e) {}

void Control::onKeyUp(KeyEventArgs & e) {}

void Control::onKeyDown(KeyEventArgs & e) {}

void Control::onGotFocus(ControlEventArgs & e) {}

void Control::onLostFocus(ControlEventArgs & e) {}

bool Control::isDragging() const
{
    return DragDrop::draggingControl() == this;
}

void Control::setDragSource(DragSource* dragSource)
{
    dragSource_ = dragSource;
}

void Control::setDropTarget(DropTarget* dropTarget)
{
    dropTarget_ = dropTarget;
}

