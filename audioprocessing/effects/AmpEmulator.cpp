#include "pch.h"

#include <effects/AmpEmulator.h>
#include <util/CalcHelper.h>

float AmpEmulator::calc(float x)
{
    // input filtration
    x = split(x, inputFilter1_, inputFilter2_, 0.75f);
    x = inputFilter3_.calc(x);

    // asimmetric limiting
    float a = lim1.calc(x);
    float b = lim2.calc(x);
    float coef = 1 + (a + b) / 4;  // 0 < coef < 1
    float y = join(a, b, coef) * 0.33f;

    // output filtration
    float resonance = resonance_.calc(y) * 1.5f;
    float middle = middle_.calc(y) * 1.7f;
    float presence = split(y, presence1_, presence2_)*0.75f;

    return y + resonance + middle + presence;
}