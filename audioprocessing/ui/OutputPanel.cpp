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
    { 255, 255, 255, 133 }    // color
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
class OutputPanel : public FxPanelImpl<OutputPanel>
{
    typedef FxPanelImpl<OutputPanel> base;

public:
    OutputPanel() {
        addControl(&volume_);
    }

protected:
    void onResized() override {
        base::onResized();

        float sc = scale();

        volume_.setScale(sc);
    }

private:
    MyRoller volume_{ 1320, 77, 0.5f };
};

REGISTER_EFFECT(OutputPanel, "Output", "Output.png");
