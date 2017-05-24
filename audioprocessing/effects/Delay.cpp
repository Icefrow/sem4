#include "pch.h"

#include <util/CalcHelper.h>
#include <effects/Delay.h>

Delay::Delay(float delay, float feedback, float wet, float fltHight)
    : delayLine_(delay)
    , feedback_(feedback)
    , wet_(wet)
    , fltHigh_(fltHight) {}

float Delay::calc(float input)
{
    float echo = delayLine_.current();
    calcAll(input + feedback_ * echo, fltLow_, fltHigh_, delayLine_);
    return join(echo, input, wet_);
}
