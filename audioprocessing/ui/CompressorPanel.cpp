#include "pch.h"

#include "FxPanel.h"
#include "CtlRoller.h"
#include "AudioProcessor.h"
#include <devices/Device.h>
#include <effects/Compressor.h>

using namespace std;

namespace {
	/* ------------------------------------------------- */
	LineMarkRollerInfo s_rollerInfo1 = {
		150,     // diameter
		22.5f,  // markWidth
		1,      // sidePadding
		25,     // centerPadding
		141,    // angle
		{ 99, 75, 33, 192 }    // color
	};

	LineMarkRollerInfo s_rollerInfo2 = {
		96,     // diameter
		15,     // markWidth
		1,      // sidePadding
		25,     // centerPadding
		141,    // angle
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

class CompressorDevice : public DeviceImpl<Compressor>
{
public:
	// Controller IDs
	enum {
		CTL_ATTACK,
		CTL_RELEASE,
		CTL_THRESHOLD,
		CTL_RATIO,
		CTL_GAIN
	};

	void devCtl(int controllerId, float value) override
	{
		switch (controllerId) {
		case CTL_ATTACK:
			effect_.set(1, 1);
			effect_.attack_ = transformValue(value, 0.025f, 1.f);
			break;

		case CTL_RELEASE:
			effect_.set(1, 1);
			effect_.release_ = transformValue(value, 0.05f, 0.45f);
			break;

		case CTL_THRESHOLD:
			effect_.threshold_ = transformValue(value, 0.f, 0.02f);
			break;

		case CTL_RATIO:
			effect_.ratio_ = transformValue(value, 1.f, 10.f);
			break;

		case CTL_GAIN:
			effect_.gain_ = transformValue(value, 1.f, 10.f);
			break;

		default:
			cout << "Invalid controller ID for Screamer device: " << controllerId << endl;
			break;
		}
	}
};

class CompressorPanel : public FxPanelImpl<CompressorPanel>
{
	typedef FxPanelImpl<CompressorPanel> base;

public:
	CompressorPanel() {
		setDevice(new CompressorDevice());

		bind(attack_, CompressorDevice::CTL_ATTACK);
		bind(release_, CompressorDevice::CTL_RELEASE);
		bind(threshold_, CompressorDevice::CTL_THRESHOLD);
		bind(ratio_, CompressorDevice::CTL_RATIO);
		bind(gain_, CompressorDevice::CTL_GAIN);
	}

private:
	MyRoller threshold_{ 233, 171, 0.5f };
	MyRoller attack_{ 578, 171, 0.5f };
	MyRoller release_{ 919, 171, 0.475f };
	MyRoller ratio_{ 1264, 171, 0.42f };
	MyRoller gain_{ 1607, 171, 0.21f };
};

REGISTER_EFFECT(CompressorPanel, "Compressor", "Compressor.png");
