#include "pch.h"

#include <effects/Screamer.h>
#include <util/CalcHelper.h>

float Screamer::calc(float input)
{
    float x = calcAll(input, inputHPF_, limiter_, outputHPF_, outputLPF_);
    return (x * clean_ + outputFilterTone_.calc(x)) * outputVolume_;
}
