#pragma once

#include <util/NonCopyable.h>
#include <ui/framework/Color.h>
#include <SFML/Graphics/Font.hpp>

class Theme sealed : NonCopyable
{
public:
    explicit Theme(const char * name, Color keyColor, Color accentColor, const char * font);

    static Theme & current();

    const sf::Font & font() const {
        return font_;
    }

    std::string name() const {
        return name_;
    }

    Color keyColor() const {
        return keyColor_;
    }

    Color accentColor() const {
        return accentColor_;
    }

private:
    Color keyColor_;
    Color accentColor_;
    const sf::Font & font_;
    std::string name_;
};

