#pragma once

#include <SFML/Graphics/Color.hpp>

class Color : public sf::Color
{
    typedef sf::Color base;

public:
    Color() : Color(255, 255, 255, 255) {}

    Color(sf::Uint8 red, sf::Uint8 green, sf::Uint8 blue, sf::Uint8 alpha = 255)
        : base(red, green, blue, alpha) {}

    explicit Color(unsigned data)
        : base(data) {}

    Color(const base & b)
        : base(b) {}

    static sf::Color fromWinColor(COLORREF colorRef) {
        Color & color = reinterpret_cast<Color &>(colorRef);
        color.a = 255;
        return color;
    }

    static Color getSysColor(int index) {
        return fromWinColor(GetSysColor(index));
    }

    static const sf::Color Gray;
};

namespace ColorDetails {
inline
sf::Uint8 clip(float value)
{
    int i = int(value);

    return i < 0 ? 0 :
           i > 255 ? 255 :
           i;
}
}   // namespace ColorDetails

inline
Color operator*(Color c, float f)
{
    return Color(ColorDetails::clip(c.r * f),
                 ColorDetails::clip(c.g * f),
                 ColorDetails::clip(c.b * f),
                 c.a);
}

inline
Color mix(Color a, Color b, float k = 0.5f)
{
    return a * k + b * (1 - k);
}