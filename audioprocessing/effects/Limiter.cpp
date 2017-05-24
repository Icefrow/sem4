#include "pch.h"
#include "Limiter.h"

void Limiter::set(float gain)
{
    gain_ = gain;
    amplitude_.set(0.001f, 0.05f);
}

float Limiter::calc(float input)
{
    float output = input * gain_;
    float ampl = amplitude_.calc(output);

    if(ampl > 0.5f)
        return 0.5f * output / ampl;

    return output;
}