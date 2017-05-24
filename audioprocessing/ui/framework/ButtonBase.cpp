#include "pch.h"
#include "ButtonBase.h"

ButtonBase::ButtonBase(ButtonReaction reaction)
    : reaction_(reaction)
    , pressed_()
    , pointerHover_(false)
{}

void ButtonBase::onPointerPressed(PointerEventArgs & e)
{
    if(e.keyModifiers() == VirtualKeyModifiers::LeftButton) {
        pressed_ = true;
        redraw();

        if(reaction_ == ButtonReaction::ByMousePress)
            onClick();
        else
            setPointerCapture();
    }
}

void ButtonBase::onPointerCaptureLost()
{
    pressed_ = false;
}

void ButtonBase::onPointerMoved(PointerEventArgs & e)
{
    bool isHover = internalBounds().contains(e.position());

    if(isHover != pointerHover_) {
        pointerHover_ = isHover;

        if(pressed_)
            redraw();
    }
}

void ButtonBase::onPointerExited()
{
    pointerHover_ = false;
}

void ButtonBase::onPointerReleased(PointerEventArgs & e)
{
    if(e.keyModifiers() == VirtualKeyModifiers::LeftButton) {
        bool wasPressed = pressed_;
        pressed_ = false;
        redraw();

        if(reaction_ != ButtonReaction::ByMouseRelease || !wasPressed)
            return;

        releasePointerCapture();

        if(isPointerHover())
            onClick();
    }
}
