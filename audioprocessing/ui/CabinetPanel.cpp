#include "pch.h"

#include "FxPanel.h"
#include "CtlRoller.h"

namespace {
/* ------------------------------------------------- */
LineMarkRollerInfo s_rollerInfo1 = {
    80,     // diameter
    10,      // markWidth
    2,      // sidePadding
    10,      // centerPadding
    137,    // angle
    { 255, 255, 255, 50 }    // color
};

LineMarkRollerInfo s_rollerInfo2 = {
    80,     // diameter
    10,      // markWidth
    10,      // sidePadding
    10,      // centerPadding
    137,    // angle
    { 255, 255, 255, 66 }    // color
};

/* ------------------------------------------------- */
class MyRoller : public LineMarkCtlRoller
{
    typedef LineMarkCtlRoller base;
public:
    // set center
    MyRoller(float x, float y, float defaultValue = 0.5)
        : base(x, y, s_rollerInfo1, s_rollerInfo2, defaultValue) {}
};

} // namespace

/* ------------------------------------------------- */
class CabinetPanel : public FxPanelImpl<CabinetPanel>
{
    typedef FxPanelImpl<CabinetPanel> base;

public:
    CabinetPanel() {
        addControls({ &volume_, &dry_ });
    }

protected:
    void onResized() override {
        base::onResized();

        float sc = scale();
        const auto & rollers = { &volume_, &dry_ };

        for(MyRoller * roller : rollers)
            roller->setScale(sc);
    }

private:
    MyRoller volume_{ 892, 159, 0.5f };
    MyRoller dry_{ 1055, 159, 0.1f };
};

REGISTER_EFFECT(CabinetPanel, "Cabinet", "Cabinet.png");