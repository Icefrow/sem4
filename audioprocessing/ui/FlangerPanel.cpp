#include "pch.h"

#include "FxPanel.h"
#include <ui/framework/ScalableControl.h>
#include <ui/framework/Roller.h>
#include "CtlRoller.h"
#include <devices/Device.h>
#include <effects/Flanger.h>

using namespace std;

namespace {
/* ------------------------------------------------- */
CircleMarkRollerInfo s_rollerInfo {
    102,    // diameter of roller
    18,     // diameter of mark circle
    20,     // offset from out side
    138,    // full angle from min position to max
    Color {27, 27, 50, 255} // markColor
};

/* ------------------------------------------------- */
class MyRoller : public CircleMarkCtlRoller
{
    typedef CircleMarkCtlRoller base;
public:
    // set center
    MyRoller(float x, float y, float defaultValue = 0.5)
        : base(x, y, s_rollerInfo, defaultValue) {}
};

} // namespace

class FlangerDevice : public DeviceImpl<Flanger>
{
public:
	// Controller IDs
	enum {
		CTL_SHIFT,
		CTL_WIDTH,
		CTL_RATE,
		CTL_REGEN,
		CTL_WET
	};

	void devCtl(int controllerId, float value) override
	{
		switch (controllerId) {
		case CTL_SHIFT:
			effect_.set(transformValue(value, 0.003f, 0.025f), effect_.deep_, effect_.freq_, effect_.wet_);
			break;

		case CTL_WIDTH:
			effect_.set(effect_.delayInSec_, value, effect_.freq_, effect_.wet_);
			break;

		case CTL_RATE:
			effect_.set(effect_.delayInSec_, effect_.deep_, transformValue(value, 0.f, 10.f), effect_.wet_);
			break;

		/*case CTL_REGEN:
			effect_.ratio_ = transformValue(value, 1.f, 10.f);
			break;*/

		case CTL_WET:
			effect_.set(effect_.delayInSec_, effect_.deep_, effect_.freq_, value);
			break;

		default:
			cout << "Invalid controller ID for Screamer device: " << controllerId << endl;
			break;
		}
	}
};

class FlangerPanel : public FxPanelImpl<FlangerPanel>
{
    typedef FxPanelImpl<FlangerPanel> base;

public:
	FlangerPanel() {
		setDevice(new FlangerDevice());

		bind(shift_, FlangerDevice::CTL_SHIFT);
		bind(width_, FlangerDevice::CTL_WIDTH);
		bind(rate_, FlangerDevice::CTL_RATE);
		bind(regen_, FlangerDevice::CTL_REGEN);
		bind(wet_, FlangerDevice::CTL_WET);
	}
private:
    MyRoller shift_{ 692, 115, 0.3f };
    MyRoller width_{ 898, 115, 0.4f };
    MyRoller rate_{ 1103, 115, 0.21f };
    MyRoller regen_{ 1317, 115, 0.275f };
    MyRoller wet_{ 1515, 115, 0.33f };
};

REGISTER_EFFECT(FlangerPanel, "Flanger", "Flanger.png");

