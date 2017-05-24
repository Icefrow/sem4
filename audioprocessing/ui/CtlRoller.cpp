#include "pch.h"
#include "CtlRoller.h"

/* ------------------------------------------------- */
CtlRoller::CtlRoller(Point center, float initialSize, float defaultValue)
    : base(Point{center.x - initialSize / 2, center.y - initialSize / 2}, Size { initialSize, initialSize })
{
    setCursor(Cursor::SizeNorthSouth);
    setValue(defaultValue);
}

void CtlRoller::onDoubleTapped(PointerEventArgs & e)
{
    setValue(0.5f); // reset to 0.5 by double click
}

/* ------------------------------------------------- */
LineMarkCtlRoller::LineMarkCtlRoller(float x, float y, const LineMarkRollerInfo & ri,
                                     float defaultValue)
    : base(Point{ x, y }, ri.size, defaultValue)
    , rollerInfo1_(ri)
{
    rollerInfo2_.size = 0;
}

LineMarkCtlRoller::LineMarkCtlRoller(float x, float y, const LineMarkRollerInfo & ri1,
                                     const LineMarkRollerInfo & ri2, float defaultValue)
    : base(Point{ x, y }, ri1.size, defaultValue)
    , rollerInfo1_(ri1)
    , rollerInfo2_(ri2) {}

void LineMarkCtlRoller::draw(Canvas & canvas, const sf::Time &)
{
    float markWidth = rollerInfo1_.markWidth * scale();
    float sidePadding = rollerInfo1_.sidePadding * scale();
    float centerPadding = rollerInfo1_.centerPadding * scale();
    float angle = value() * rollerInfo1_.angle * 2 - rollerInfo1_.angle;

    drawLineMark(canvas, markWidth, sidePadding, centerPadding, angle, rollerInfo1_.markColor);

    if(rollerInfo2_.size) {
        markWidth = rollerInfo2_.markWidth * scale();
        sidePadding = rollerInfo2_.sidePadding * scale();
        centerPadding = rollerInfo2_.centerPadding * scale();
        angle = value() * rollerInfo2_.angle * 2 - rollerInfo2_.angle;

        drawLineMark(canvas, markWidth, sidePadding, centerPadding, angle, rollerInfo2_.markColor);
    }
}

/* ------------------------------------------------- */
CircleMarkCtlRoller::CircleMarkCtlRoller(float x, float y, const CircleMarkRollerInfo & ri,
                                         float defaultValue)
    : base(Point{ x, y }, ri.size, defaultValue)
    , rollerInfo_(ri)
{
}

void CircleMarkCtlRoller::draw(Canvas & canvas, const sf::Time &)
{
    float markWidth = rollerInfo_.markSize * scale();
    float paddingExternal = rollerInfo_.sidePadding * scale();
    float maxAngle = rollerInfo_.angle;
    float angle = value() * maxAngle * 2 - maxAngle - 0.5f;

    drawCircleMark(canvas, markWidth, paddingExternal, angle, rollerInfo_.markColor);
}
