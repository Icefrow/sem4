#pragma once

#include <dsp/Filters.h>
#include <dsp/Limiters.h>
#include <util/CalcHelper.h>

// Popular overdrive-like device
// http://www.electrosmash.com/images/tech/tube-screamer/tube-screamer-true-bypass-schematic.png
struct Screamer {
    float calc(float input);

    float dryLevel_ = 1;
    Hpf1 inputHPF_{ 100 };
    Lpf1 inputLPF_{ 3500 };
    LimiterByDividing limiter_{ 120, 0.00001f };
    Hpf1 outputHPF_{ 4545 };
    Lpf1 outputLPF_{ 4545 };
    Lpf2 outputFilterTone_{ 2200, 3.7f };
    float clean_ = 0.5f;
    float outputVolume_ = 0.5f;
};
