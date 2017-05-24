#pragma once

#include <dsp/DelayLine.h>
#include "AmpEmulator.h"

class Reverberator
{
public:
    Reverberator(float wet = 0.33f);

    float calc(float input);

private:
    float wet_;

    DelayLine d0{ 0.050f }; // predelay
    DelayLine d1{ 0.003f };
    DelayLine d2{ 0.005f };
    DelayLine d3{ 0.007f };
    DelayLine d4{ 0.011f };
    DelayLine d5{ 0.013f };
    DelayLine d6{ 0.017f };
    DelayLine d7{ 0.019f };
    DelayLine d8{ 0.021f };

    Hpf2 low_{ 250, 0.5f };
    Lpf2 high_{ 2700, 0.5f };
};

