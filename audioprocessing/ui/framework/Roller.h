#pragma once

#include <functional>
#include <ui/framework/Control.h>

class Roller : public Control
{
public:
    float value() const {
        return value_;
    }

    void setValue(float newValue);

    std::function<void(float value)> eventValueChanged;

    float sensitivity() const {
        return sensitivity_;
    }

    void setSensitivity(float newSensitivity) {
        sensitivity_ = newSensitivity;
    }

protected:
    void onPointerPressed(PointerEventArgs & e) override;
    void onPointerMoved(PointerEventArgs & e)override;
    void onPointerReleased(PointerEventArgs & e) override;

    // Vertical mark is 0 degree
    // derived class will paint mark with needed parameters
    void drawLineMark(Canvas canvas, float width, float paddingExternal, float paddingInternal, float angle, Color color);

    // Vertical mark is 0 degree
    // derived class will paint mark with needed parameters
    void drawCircleMark(Canvas canvas, float size, float paddingExternal, float angle, Color color);

 private:
    float value_ = 0.5f;        // range 0 .. 1
    float sensitivity_ = 1;
    float hitY_ = 0;
    float hitValue_ = 0;
};

