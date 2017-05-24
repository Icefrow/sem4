#include "pch.h"

#include "FxPanel.h"
#include "CtlRoller.h"
#include "AudioProcessor.h"
#include <devices/Device.h>
#include <effects/Screamer.h>

using namespace std;

namespace {
/* ------------------------------------------------- */
LineMarkRollerInfo s_rollerInfo1 = {
    96,     // diameter
    22.5f,  // markWidth
    1,      // sidePadding
    25,     // centerPadding
    148,    // angle
    { 99, 75, 33, 192 }    // color
};

LineMarkRollerInfo s_rollerInfo2 = {
    96,     // diameter
    15,     // markWidth
    1,      // sidePadding
    25,     // centerPadding
    148,    // angle
    { 192, 255, 255, 133 }    // color
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

class ScreamerDevice : public DeviceImpl<Screamer>
{
public:
	// Controller IDs
	enum {
		CTL_BASS,
		CTL_TREBLE,
		CTL_TONE,
		CTL_DRIVE,
		CTL_CLEAN,
		CTL_VOLUME
	};

	void devCtl(int controllerId, float value) override
	{
		switch (controllerId) {
		case CTL_BASS:
			effect_.inputHPF_.set(transformValue(value, 750, 100));
			effect_.outputHPF_.set(transformValue(value, 5000, 750));
			break;

		case CTL_TREBLE:
			effect_.inputLPF_.set(transformValue(value, 750, 5000));
			effect_.outputLPF_.set(transformValue(value, 2700, 6500));
			break;

		case CTL_DRIVE:
			effect_.limiter_.set(transformValue(value, 20, 400), 0.00001f);
			break;

		case CTL_TONE:
			effect_.outputFilterTone_.set(
				transformValue(value, 1500, 4000),      // Frequency
				transformValue(value, 1.41f, 3.5f));    // Quality factor
			break;

		case CTL_CLEAN:
			effect_.clean_ = value;
			break;

		case CTL_VOLUME:
			effect_.outputVolume_ = value;
			break;

		default:
			cout << "Invalid controller ID for Screamer device: " << controllerId << endl;
			break;
		}
	}
};

class ScreamerPanel : public FxPanelImpl<ScreamerPanel>
{
    typedef FxPanelImpl<ScreamerPanel> base;

public:
    ScreamerPanel() {
        setDevice(new ScreamerDevice());

        bind(bass_, ScreamerDevice::CTL_BASS);
        bind(treble_, ScreamerDevice::CTL_TREBLE);
        bind(drive_, ScreamerDevice::CTL_DRIVE);
        bind(tone_, ScreamerDevice::CTL_TONE);
        bind(clean_, ScreamerDevice::CTL_CLEAN);
        bind(volume_, ScreamerDevice::CTL_VOLUME);
    }

private:
    MyRoller bass_{ 626, 148, 0.5f };
    MyRoller treble_{ 825, 148, 0.5f };
    MyRoller drive_{ 1022, 148, 0.475f };
    MyRoller tone_{ 1221, 148, 0.42f };
    MyRoller clean_{ 1420, 148, 0.21f };
    MyRoller volume_{ 1618, 148, 0.42f };
};

REGISTER_EFFECT(ScreamerPanel, "Screamer", "Screamer.png");
