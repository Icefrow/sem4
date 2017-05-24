#pragma once

#include <ui/framework/ButtonBase.h>
#include <SFML/Graphics/Text.hpp>
#include <functional>

class Button : public ButtonBase
{
    typedef ButtonBase base;

public:
    explicit Button(const char * text = "", ButtonReaction reaction = ButtonReaction::ByMouseRelease);

    void setString(const char * text);

    std::function<void()> eventClick;

protected:
    const sf::Text & text() const {
        return text_;
    }

    void onClick() override;

    void onPointerEntered() override;

    void onPointerExited() override;

    void draw(Canvas & canvas, const sf::Time & elapsedTime) override;

    virtual void drawBackgound(Canvas & canvas, const sf::Time &);

    virtual void drawForeground(Canvas & canvas, const sf::Time &);

    void onResized() override;

private:
    sf::Text text_;
    Point textPosition_;
};

