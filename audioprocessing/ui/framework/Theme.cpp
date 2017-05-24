#include "pch.h"

#include <memory>

#include "Theme.h"
#include "ResourceManager.h"

namespace {
std::unique_ptr<Theme> s_Dark;

//Theme s_SlightDark("Slight Dark", Color(117, 127, 111));
//Theme s_Light("Light", Color(199, 205, 175));
//Theme s_Sand("Sand", Color(177, 192, 133));

Theme * s_currentTheme = nullptr;

void loadThemes()
{
    s_Dark.reset(new Theme("Dark", Color(76, 76, 76), Color(192, 192, 33), "Helvetica"));

    s_currentTheme = s_Dark.get();
}
}   // namespace

Theme::Theme(const char * name, Color keyColor, Color accentColor, const char * font)
    : keyColor_(keyColor)
    , accentColor_(accentColor)
    , font_(ResourceManager::getFont(font))
    , name_(name) {}

Theme & Theme::current()
{
    if(!s_currentTheme)
        loadThemes();

    return *s_currentTheme;
}
