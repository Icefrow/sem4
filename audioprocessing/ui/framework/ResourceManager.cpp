#include "pch.h"
#include "ResourceManager.h"

#include <util/ResourceLoader.h>

#include <map>
#include <memory>

using namespace std;

namespace {
typedef unique_ptr<Texture> TexturePtr;
typedef map<string, TexturePtr> TextureMap;
TextureMap s_textures;

typedef unique_ptr<Font> FontPtr;
typedef map<string, FontPtr> FontMap;
FontMap s_fonts;

}   // namespace

Texture * ResourceManager::getTexture(const char * textureName)
{
    auto it = s_textures.find(textureName);

    if(it != s_textures.end())
        return it->second.get();

    Blob resource = loadResourceData(textureName, "TEXTURE");

    if(!resource.data)
        throw runtime_error(string("Can not find texture ") + textureName);

    unique_ptr<Texture> texturePtr = make_unique<Texture>();

    sf::Image image;

    if(!image.loadFromMemory(resource.data, resource.size))
        throw runtime_error(string("Can not load image ") + textureName);

    if(textureName[0] == '~')
        image.createMaskFromColor(sf::Color(255, 0, 255));

    if(!texturePtr->loadFromImage(image))
        throw runtime_error(string("Can not load texture ") + textureName + " to video memory");

    Texture * result = texturePtr.get();
    result->setSmooth(true);
    result->generateMipmap();

    s_textures[textureName] = move(texturePtr);
    return result;
}

const Font & ResourceManager::getFont(const char * fontName)
{
    auto it = s_fonts.find(fontName);

    if(it != s_fonts.end())
        return *it->second;

    Blob resource = loadResourceData(fontName, "EMBEDFONT");

    if(!resource.data)
        throw runtime_error(string("Can not find font ") + fontName);

    unique_ptr<Font> fontPtr = make_unique<Font>();

    if(!fontPtr->loadFromMemory(resource.data, resource.size))
        throw runtime_error(string("Can not load font ") + fontName);

    Font * result = fontPtr.get();

    s_fonts[fontName] = move(fontPtr);
    return *result;
}

void ResourceManager::clear()
{
    s_textures.clear();
    s_fonts.clear();
}
