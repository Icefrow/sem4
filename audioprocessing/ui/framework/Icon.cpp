#include "pch.h"
#include "Icon.h"

Icon::Icon(sf::Texture * texture, const Rect & textureRect)
    : texture_(texture)
    , textureRect_(textureRect)
{
    assert(texture);
}

