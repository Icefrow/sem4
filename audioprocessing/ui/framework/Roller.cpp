#include "pch.h"

#include <ui/framework/Roller.h>

void Roller::setValue(float newValue)
{
    if(newValue < 0)
        newValue = 0;
    else if(newValue > 1.0f)
        newValue = 1.0f;

    if(value_ != newValue) {
        value_ = newValue;

        redraw();

        if(eventValueChanged)
            eventValueChanged(newValue);
    }
}

void Roller::onPointerPressed(PointerEventArgs & e)
{
    if (e.keyModifiers() == VirtualKeyModifiers::LeftButton) {
        hitY_ = e.position().y;
        hitValue_ = value_;

        setPointerCapture();
    }
}

void Roller::onPointerMoved(PointerEventArgs & e)
{
    if(e.keyModifiers() == VirtualKeyModifiers::LeftButton && getPointerCapture() == this) {
        float delta = (hitY_ - e.position().y) * sensitivity_ * 0.01f;
        setValue(hitValue_ + delta);
    }
}

void Roller::onPointerReleased(PointerEventArgs & e)
{
    if(e.keyModifiers() == VirtualKeyModifiers::LeftButton) {
        if(getPointerCapture() == this)
            releasePointerCapture();
    }
}

void Roller::drawLineMark(Canvas canvas, float width,
                          float paddingExternal, float paddingInternal,
                          float angle, Color color)
{
    Rect r = internalBounds();

    // center
    float x = r.width() / 2;
    float y = r.height() / 2;
    Rect marker = {{-width * 0.5f, paddingExternal - y}, {width, y - paddingExternal - paddingInternal}};

    // marker at 90 degree == value 0.5
    canvas.transform().translate(x, y);
    canvas.transform().rotate(angle);

    canvas.horizontalGradient(marker, 0.5, color * 0.5, color * 1.5, color * 0.5);
}

void Roller::drawCircleMark(Canvas canvas, float size, float paddingExternal, float angle,
                           Color color)
{
    Rect r = internalBounds();

    // center
    float x = r.width() / 2;
    float y = r.height() / 2;
    Rect marker = { Point{ -size * 0.5f, paddingExternal - y}, Size{size, size} };

    // marker at 90 degree == value 0.5
    canvas.transform().translate(x, y);
    canvas.transform().rotate(angle);

    canvas.fillEllipse(marker, color);
}
