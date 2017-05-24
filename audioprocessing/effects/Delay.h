#pragma once
#include <dsp/Filters.h>
#include <dsp/DelayLine.h>

class Delay
{
public:
    explicit Delay(float delay = 0.475f, float feedback = 0.33f,
                   float wet = 0.25f, float fltHight = 3500);

    float calc(float input);

private:
    DelayLine delayLine_;
    float feedback_;
    float wet_;

    Hpf1 fltLow_ { 750 };
    Lpf1 fltHigh_ { 3500 };
};

