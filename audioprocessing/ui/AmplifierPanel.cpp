#include "pch.h"

#include "FxPanel.h"

#include <ui/framework/ToggleButton.h>
#include <ui/framework/Roller.h>
#include <ui/framework/ScalableControl.h>
#include "CtlRoller.h"

namespace {

/* ------------------------------------------------- */
class HighLowKnob : public ScalableControl<ToggleButton>
{
    typedef ScalableControl<ToggleButton> base;

public:
    HighLowKnob(float x, float y)
        : HighLowKnob(x, y, ResourceManager::getTexture("AmplifierKnobHigh.png")) {}

protected:
    void draw(Canvas & canvas, const sf::Time &) override {
        if(state())
            canvas.fill(internalBounds(), knobHighTexture_, scale());
    }

private:
    HighLowKnob(float x, float y, Texture * texture)
        : base(Point{x, y}, texture->getSize())
        , knobHighTexture_(texture) {
        setCursor(Cursor::Hand);
    }

    Texture * knobHighTexture_;
};

/* ------------------------------------------------- */
LineMarkRollerInfo s_rollerInfo = {
    100,        // diameter
    8,          // markWidth
    13,         // sidePadding
    5,          // centerPadding
    138,        // angle
    { 10, 5, 0, 166 }    // color
};

/* ------------------------------------------------- */
class MyRoller : public LineMarkCtlRoller
{
    typedef LineMarkCtlRoller base;
public:
    // set center
    MyRoller(float x, float y, float defaultValue = 0.5)
        : base(x, y, s_rollerInfo, defaultValue) {}
};

} // namespace

/* ------------------------------------------------- */
class AmplifierPanel : public FxPanelImpl<AmplifierPanel>
{
    typedef FxPanelImpl<AmplifierPanel> base;

public:
    AmplifierPanel() {
        addControls({
            &hlKnob_, &gain_, &bass_, &middle_,
            &treble_, &depth_, &presence_, &master_
        });
    }

protected:
    void onResized() override {
        base::onResized();

        float sc = scale();

        const auto & rollers = {
            &gain_, &bass_, &middle_, &treble_,
            &depth_, &presence_, &master_
        };

        for(MyRoller * roller : rollers)
            roller->setScale(sc);

        hlKnob_.setScale(sc);
    }

private:
    HighLowKnob hlKnob_ {355, 79};
    MyRoller gain_ {561, 126, 0.375f};
    MyRoller bass_ {740, 126, 0.64f};
    MyRoller middle_ {920, 126};
    MyRoller treble_ {1100, 126, 0.645f};
    MyRoller depth_ {1279, 126, 0.27f};
    MyRoller presence_ {1458, 126, 0.625f};
    MyRoller master_ {1636, 126, 0.61f};
};

REGISTER_EFFECT(AmplifierPanel, "Amplifier", "Amplifier.png");

