#pragma once

#include "ui/framework/ScalableControl.h"
#include "ui/framework/Roller.h"

/* ------------------------------------------------- */
// Base roller class for effect controller
class CtlRoller : public ScalableControl<Roller>
{
    typedef ScalableControl<Roller> base;

public:
    CtlRoller(Point center, float initialSize, float defaultValue = 0.5f);

protected:
    void onDoubleTapped(PointerEventArgs & e) override;
};

/* ------------------------------------------------- */
struct LineMarkRollerInfo {
    float size;             // diameter of roller
    float markWidth;        // width of mark line
    float sidePadding;      // offset from out side
    float centerPadding;    // offset from center
    float angle;            // full angle from min position to max
    Color markColor;
};

// Roller class that draw line mark
class LineMarkCtlRoller : public CtlRoller
{
    typedef CtlRoller base;

public:
    LineMarkCtlRoller(float x, float y, const LineMarkRollerInfo & ri,
                      float defaultValue = 0.5f);

    LineMarkCtlRoller(float x, float y, const LineMarkRollerInfo & ri1,
                      const LineMarkRollerInfo & ri2, float defaultValue = 0.5f);

protected:
    void draw(Canvas & canvas, const sf::Time &) override;

private:
    LineMarkRollerInfo rollerInfo1_;
    LineMarkRollerInfo rollerInfo2_;
};

/* ------------------------------------------------- */
struct CircleMarkRollerInfo {
    float size;             // diameter of roller
    float markSize;         // diameter of mark circle
    float sidePadding;      // offset from out side
    float angle;            // full angle from min position to max
    Color markColor;
};

// Roller class that draw circle mark
class CircleMarkCtlRoller : public CtlRoller
{
    typedef CtlRoller base;

public:
    CircleMarkCtlRoller(float x, float y, const CircleMarkRollerInfo & ri,
                        float defaultValue = 0.5f);

protected:
    void draw(Canvas & canvas, const sf::Time &) override;

private:
    CircleMarkRollerInfo rollerInfo_;
};
