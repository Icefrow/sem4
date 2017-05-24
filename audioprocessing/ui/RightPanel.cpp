#include "pch.h"

#include <ui/framework/Theme.h>
#include "RightPanel.h"

using namespace std;

namespace {
constexpr float DefaultWidth = 1920;
constexpr float padding = 6;
constexpr float TopPanelHeight = 33;
}   // namespace

void RightPanel::Toolbox::drawBackground(Canvas & canvas, const sf::Time &)
{
    Color keyColor = Theme::current().keyColor() * 0.75f;
    canvas.verticalGradient(internalBounds(), 0.892f,
                            keyColor * 0.75f, keyColor * 0.85f, keyColor * 0.000006f);
}

RightPanel::RightPanel()
    : btnOpen_("Open")
    , btnSave_("Save")
    , btnSettings_("Settings")
{
    initControls();
}

void RightPanel::initControls()
{
    float x = padding;
    float y = 2;
    const float buttonWidth = 66;
    const float buttonHeight = TopPanelHeight - 6;

    btnOpen_.setBounds({ x, y, buttonWidth, buttonHeight });
    x += buttonWidth + padding;

    btnSave_.setBounds({ x, y, buttonWidth, buttonHeight });
    x += buttonWidth + padding;

    btnSettings_.setSize({ buttonWidth, buttonHeight });
    btnSettings_.setPosition({ x, y });

    addControls({ &toolbox_, &fxRack_ });

    toolbox_.addControls({ &btnOpen_, &btnSave_, &btnSettings_ });

    fxRack_.setPosition({ 0, TopPanelHeight });
    setSize({ 500, 600 });
}

void RightPanel::onResized()
{
    toolbox_.setSize({ width(), TopPanelHeight });

    Rect r = btnSettings_.bounds();
    btnSettings_.setPosition({width() - r.width() - padding, r.top()});

    fxRack_.setSize({width(), height() - fxRack_.position().y});
}

void RightPanel::drawBackground(Canvas & canvas, const sf::Time &)
{}