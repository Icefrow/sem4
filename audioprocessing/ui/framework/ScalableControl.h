#pragma once

#include <ui/framework/Coordinates.h>

/* ------------------------------------------------------- */
struct IScalableControl
{
    virtual ~IScalableControl() {}
    virtual void setScale(float scale) = 0;
};

/* ------------------------------------------------------- */
template<typename Base>
class ScalableControl : public Base, public IScalableControl
{
public:
    ScalableControl(Point initialPosition, Size initialSize)
        : initialPosition_(initialPosition)
        , initialSize_(initialSize) {}

    void setScale(float scale) override {
        scale_ = scale;
        Base::setBounds(Rect{initialPosition_, initialSize()} * scale);
    }

    float scale() const {
        return scale_;
    }

protected:
    const Size & initialSize() const {
        return initialSize_;
    }

private:
    Point initialPosition_;
    Size initialSize_;
    float scale_ = 1;
};

