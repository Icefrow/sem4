#include "pch.h"
#include "NoiseGate.h"
#include <util/CalcHelper.h>

NoiseGate::NoiseGate(float level, float attackInSec, float releaseInSec)
    : ar_(attackInSec, releaseInSec)
    , gate_()
    , level_(level)
{}

void NoiseGate::set(float level, float attackInSec, float releaseInSec)
{
    level_ = level;
    ar_.set(attackInSec, releaseInSec);
}

float NoiseGate::calc(float input)
{
    float x = calcAll(input * (gate_ + 0.1f), fltInput1_, fltInput2_, fltInput3_);
    x = abs(x) / level_ - level_ / 2.f;

    if(x > 1)
        x = 1;
    else if(x < 0)
        x = 0;

    float y1 = ar_.calc(x);
    gate_ = fltEnvelope_.calc(y1);

    return input * gate_;
}

