#include "pch.h"
#include "Flanger.h"

#include <util/AudioConverter.h>
#include <util/CalcHelper.h>

Flanger::Flanger(float delay, float deep, float freq, float wet)
    : delayLine_(MaxDelay * 2)
{
    set(delay, deep, freq, wet);
}

void Flanger::set(float delayInSec, float deep, float freq, float wet)
{
	deep_ = deep;
	freq_ = freq;
	delayInSec_ = delayInSec;
    delay_ = limitValue(delayInSec, MinDelay, MaxDelay);
    modulationAmplitude_ = limitValue(deep, 0, 0.9999f) * delay_;
    generator_.set(limitValue(freq, 0, 10));    // 0..10 Hz
    wet_ = limitValue(wet, 0, 1);
}

float Flanger::calc(float input)
{
    float currentDelay = delay_ + generator_.calc() * modulationAmplitude_;
    float y = delayLine_.calc(input, currentDelay);
    return join(y, input, wet_);
}
