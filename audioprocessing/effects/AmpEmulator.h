#pragma once

#include <dsp/Limiters.h>
#include <dsp/Filters.h>

class AmpEmulator
{
public:
    float calc(float input);

private:
    Hpf1 inputFilter1_{ 50 };
    Lpf2 inputFilter2_{ 150, 1.41f };
    Hpf1 inputFilter3_{ 5500 };

    RootLimiter lim1 { 0.75f, +0.0005f };
    RootLimiter lim2 { 0.83f, -0.0005f };

    Bpf2 middle_{ 650 };
    Hpf1 presence1_{ 4100 };
    Hpf2 presence2_{ 4300 };
    Lpf2 resonance_{ 110, 2.7f };
};

