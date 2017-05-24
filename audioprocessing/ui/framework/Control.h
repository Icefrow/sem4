#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>

#include <ui/framework/ControlEvents.h>
#include <ui/framework/Canvas.h>
#include <ui/framework/Cursor.h>
#include <ui/framework/HostWindow.h>

class Control : NonCopyable
{
public:
    explicit Control(Rect bounds = {});

    virtual ~Control();

    Control * parent() const {
        return parent_;
    }

    // parent control coordinate system
    const Rect bounds() const {
        return {position_, size_};
    }

    // this control coordinate system
    const Rect internalBounds() const {
        return {Point{0, 0}, size_};
    }

    void setBounds(const Rect & newBounds) {
        setPosition(newBounds.position());
        setSize(newBounds.size());
    }

    Point position() const {
        return position_;
    }

    void setPosition(Point newPos) {
        position_ = newPos;
    }

    Size size() const {
        return size_;
    }

    void setSize(Size newsSize);

    float width() const {
        return size_.width();
    }

    void setWidth(float newWidth) {
        setSize({ newWidth, height() });
    }

    float height() const {
        return size_.height();
    }

    void setHeight(float newHeight) {
        setSize({ width(), newHeight });
    }

    float left() const {
        return position_.x;
    }

    float top() const {
        return position_.y;
    }

    float right() const {
        return position_.x + size_.width();
    }

    float bottom() const {
        return position_.y + size_.height();
    }

    Size minSize() const {
        return minSize_;
    }

    void setMinSize(Size minSize);

    Size maxSize() const {
        return maxSize_;
    }

    bool enabled() const {
        return enabled_;
    }

    void setEnabled(bool value);

    Cursor getCursor() const;

    void setCursor(Cursor cursor);

    // request window to redraw content
    void redraw();

    bool isDragging() const;

    DragSource * dragSource() const {
        return dragSource_;
    }

    void setDragSource(DragSource * dragSource);

    DropTarget * dropTarget() const {
        return dropTarget_;
    }

    virtual void setDropTarget(DropTarget * dropTarget);

protected:
    HostWindow * hostWindow() {
        return hostWindow_;
    }

    bool isPointerHover() const {
        if(HostWindow * hw = hostWindow_)
            return hw->pointerHoverCtl_ == this;

        return false;
    }

    void hostToClient(Point & position);

    // set pointer capture to this controls
    void setPointerCapture();

    // release previously captured pointer
    void releasePointerCapture();

    // get current control (or NULL) that captured pointer input
    Control * getPointerCapture() const;

    //HostWindow * hostWindow() const {
    //    return hostWindow_;
    //}

    virtual void onResized();

    //bool focused() const;

    virtual void onPointerEntered();
    virtual void onPointerMoved(PointerEventArgs & e);
    virtual void onPointerExited();

    virtual void onPointerPressed(PointerEventArgs & e);
    virtual void onPointerReleased(PointerEventArgs & e);

    virtual void onDoubleTapped(PointerEventArgs & e);

    virtual void onPointerCaptureLost();
    virtual void onPointerCanceled(PointerEventArgs & e);
    virtual void onPointerWheelChanged(PointerEventArgs & e);

    virtual void onKeyUp(KeyEventArgs & e);
    virtual void onKeyDown(KeyEventArgs & e);
    virtual void onGotFocus(ControlEventArgs & e);
    virtual void onLostFocus(ControlEventArgs & e);

    void addChildInternal(Control * child);
    void removeChildInternal(Control * child);

    virtual void draw(Canvas & canvas, const sf::Time & elapsedTime) = 0;

    Control * getControlAtPoint(Point & pt);

private:
    void setHostWindow(HostWindow * newWindow);

    Point position_;
    Size size_;
    Size minSize_;
    Size maxSize_;
    bool enabled_ = true;
    Cursor cursor_ = Cursor::Default;

    Control * parent_ = nullptr;
    Control * next_ = nullptr;  // downward by z-order
    Control * firstChild_ = nullptr;

    HostWindow * hostWindow_ = nullptr;
    DragSource * dragSource_ = nullptr;
    DropTarget * dropTarget_ = nullptr;

    friend class HostWindow;
    friend class Panel;

    friend class FxRack;
    friend class RightPanel;
};

