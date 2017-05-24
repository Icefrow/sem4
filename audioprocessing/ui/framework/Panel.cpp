#include "pch.h"
#include "Panel.h"
#include "Theme.h"

Panel::Panel()
{
}

void Panel::addControl(Control * child)
{
    addChildInternal(child);
}

void Panel::draw(Canvas & canvas, const sf::Time & elapsedTime)
{
    drawBackground(canvas, elapsedTime);

    Control * child = firstChild_;

    while(child) {
        Canvas childCanvas = canvas;
        childCanvas.transform().translate(child->position());
        child->draw(childCanvas, elapsedTime);
        child = child->next_;
    }
}

void Panel::drawBackground(Canvas & canvas, const sf::Time &)
{
    //canvas.transform().translate(1, 1);

    //Color btnColor = Color::getSysColor(COLOR_BTNFACE);
    //Color shadow = Color::getSysColor(COLOR_BTNSHADOW);
    //Color dkshadow = Color::getSysColor(COLOR_3DDKSHADOW);

    Rect rect = internalBounds();
    canvas.fill(rect, Theme::current().keyColor()*0.5);
}

void TransparentPanel::drawBackground(Canvas &, const sf::Time &)
{}
