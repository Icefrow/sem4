#include "pch.h"
#include "Button.h"
#include "Theme.h"

Button::Button(const char * text, ButtonReaction reaction)
    : base(reaction)
{
    text_.setString(text);
    text_.setFont(Theme::current().font());
    text_.setCharacterSize(13);
    text_.setFillColor(mix(Color::White, Theme::current().accentColor()));
    text_.setOutlineThickness(0.2f);
}

void Button::setString(const char * text)
{
    text_.setString(text);
}

void Button::onResized()
{
    Rect textRect = text_.getLocalBounds();
    float chHeight = float(text_.getCharacterSize());    // in pixels

    Point textCenter = Rect(textRect.left(), textRect.top(), textRect.width(), chHeight)
                       .center();

    Point buttonCenter = internalBounds().center();
    textPosition_ = {round(buttonCenter.x - textCenter.x), round(buttonCenter.y - textCenter.y + 2.f)};

    base::onResized();
}

void Button::drawBackgound(Canvas & canvas, const sf::Time &)
{
    Theme & theme = Theme::current();
    Color keyColor = theme.keyColor();
    Color accentColor = theme.accentColor();

    Color bkColor = pressed() && pointerHover() ?
                    mix(keyColor * 0.39, accentColor * 0.75f, 0.95f) :
                    pointerHover() ?
                    mix(keyColor * 0.42, accentColor * 0.75f, 0.91f) :
                    keyColor * .5f;

    canvas.verticalGradient(internalBounds() - Size(2, 2), 0.2f,
                            bkColor * 1.65f, bkColor * 1.75f, bkColor * 1.15f);

    Color boundsColor = isPointerHover()
                        ? mix(keyColor * 0.25, accentColor, 0.33f)
                        : mix(keyColor * 0.25, accentColor, 0.66f);

    canvas.rect(internalBounds(), boundsColor);
}

void Button::drawForeground(Canvas & canvas, const sf::Time &)
{
    if(pressed() && pointerHover())
        text_.setPosition(textPosition_ + Point(0, 1));
    else
        text_.setPosition(textPosition_);

    canvas.draw(text_);
}

void Button::onClick()
{
    if(eventClick)
        eventClick();
}

void Button::onPointerEntered()
{
    base::onPointerEntered();
    redraw();
}

void Button::onPointerExited()
{
    base::onPointerExited();
    redraw();
}

void Button::draw(Canvas & canvas, const sf::Time & time)
{
    drawBackgound(canvas, time);
    drawForeground(canvas, time);
}
