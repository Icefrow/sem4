#include "pch.h"

#include "FxPanel.h"
#include "CtlRoller.h"

namespace {
/* ------------------------------------------------- */
LineMarkRollerInfo s_rollerInfo = {
    80,     // diameter
    6,      // markWidth
    8,      // sidePadding
    8,      // centerPadding
    137,    // angle
    {255, 255, 255, 133}    // color
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
class InputPanel : public FxPanelImpl<InputPanel>
{
    typedef FxPanelImpl<InputPanel> base;

public:
    InputPanel() {
        addControls({ &level_, &noise_});
    }

protected:
    void onResized() override {
        base::onResized();

        float sc = scale();
        const auto & list = { &level_, &noise_};

        for(MyRoller * ctl : list)
            ctl->setScale(sc);
    }

private:
    MyRoller level_{ 618, 79, 0.5f };
    MyRoller noise_{ 1312, 79, 0.4f };
};

REGISTER_EFFECT(InputPanel, "Input", "~Input.png");
