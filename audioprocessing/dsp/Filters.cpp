#include "pch.h"

#include <dsp/Globals.h>
#include <dsp/Filters.h>
#include <util/CalcHelper.h>

/* ------------------------------------------------- */
void LowPassFilter2::set(float f, float q)
{
    float w = 2 * PI * f;
    float R = 1;
    float L = R / (w * q);
    float C = 1 / (L * w * w);

    c_.capacity(C);
    l_.inductance(L);
}

float LowPassFilter2::calc(float u)
{
    float u_r = c_.potential();
    float u_l = u - u_r;
    float i_l = l_.calc(u_l);
    float i_c = i_l - u_r; // /R=1
    c_.calc(i_c);
    return u_r;
}

/* ------------------------------------------------- */
void HighPassFilter2::set(float f, float q)
{
    l_.inductance(1.0f / (f * f * c_.capacity() * 4 * PI * PI));
    r_ = l_.inductance() * f * 2 * PI / q;
}

float HighPassFilter2::calc(float u)
{
    float u_l = u - c_.potential();
    float u_r = u_l;
    l_.calc(u_l);
    float i_l = l_.current();
    float i_c = i_l + u_r / r_;
    c_.calc(i_c);
    return u_l;
}

/* ------------------------------------------------- */
void BandPassFilter2::set(float f, float q)
{
    low_.set(f, q);
    high_.set(f, q);
}

float BandPassFilter2::calc(float input)
{
    return calcAll(input, low_, high_);
}
