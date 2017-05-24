#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <ui/framework/Coordinates.h>

using Texture = sf::Texture;

// Represents icon
class Icon
{
public:
    Icon(sf::Texture * texture, const Rect & textureRect);

    const sf::Texture * texture() const {
        return texture_;
    }

    const Rect & textureRect() const {
        return textureRect_;
    }

private:
    sf::Texture * texture_;
    Rect textureRect_;
};

