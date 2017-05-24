#include "pch.h"

#include "FxPanel.h"
#include "CtlRoller.h"

namespace {
/* ------------------------------------------------- */
CircleMarkRollerInfo s_widthRollerInfo = {
    90,     // diameter of roller
    15,     // diameter of mark circle
    4,      // offset from out side
    138,    // full angle from min position to max
    Color {0, 33, 128, 110} // markColor
};

CircleMarkRollerInfo s_heightRollerInfo = {
    90,     // diameter of roller
    15,     // diameter of mark circle
    4,     // offset from out side
    138,    // full angle from min position to max
    Color {128, 0, 128, 110} // markColor
};
} // namespace

class OscilloscopePanel : public FxPanelImpl<OscilloscopePanel>
{
    typedef FxPanelImpl<OscilloscopePanel> base;

public:
    OscilloscopePanel() {
        addControls({ &width_, &height_ });
    }

protected:
    void onResized() override {
        base::onResized();

        float sc = scale();
        const auto & rollers = { &width_, &height_ };

        for(auto * roller : rollers)
            roller->setScale(sc);
    }

private:
    CircleMarkCtlRoller width_{ 388, 148, s_widthRollerInfo, 0.75f };
    CircleMarkCtlRoller height_{ 1669, 147, s_heightRollerInfo, 0.65f };
};

REGISTER_EFFECT(OscilloscopePanel, "Oscilloscope", "Oscilloscope.png");
