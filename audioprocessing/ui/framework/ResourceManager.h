#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

using Texture = sf::Texture;
using Font = sf::Font;

class ResourceManager sealed 
{
public:
    static Texture * getTexture(const char * textureName);

    static const Font & getFont(const char * fontName);

    static void clear();

private:
    ResourceManager() = delete;
    ~ResourceManager() = delete;
};

