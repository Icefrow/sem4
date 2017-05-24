#pragma once

#include <effects/AmplitudeDetector.h>
#include "dsp/Filters.h"

class NoiseGate
{
public:
    explicit NoiseGate(float level = 0.005, float attackInSec = 0.015f,
                       float releaseInSec = 0.500f);

    void set(float level = 0.005, float attackInSec = 0.015f, float releaseInSec = 0.500f);

    float calc(float input);

private:
    AttackRelease ar_;
    float gate_;
    float level_;

    Lpf2 fltInput1_ { 1700, 0.7f };
    Hpf2 fltInput2_ { 100, 0.7f };
    Hpf1 fltInput3_{ 500 };

    Lpf2 fltEnvelope_{ 60, 0.7f };
};

