#include "pch.h"

#include <util/CalcHelper.h>

#include "Reverberator.h"

Reverberator::Reverberator(float wet)
    : wet_(wet) {}

float Reverberator::calc(float input)
{
    float output = calcAll(input, low_, high_);

    float y = d0.calc(input);

    y = d1.calc(y);
    output += y;
    y = d2.calc(y);
    output += y;
    y = d3.calc(y);
    output += y;
    y = d4.calc(y);
    output += y;
    y = d5.calc(y);
    output += y;
    y = d6.calc(y);
    output += y;
    y = d7.calc(y);
    output += y;
    y = d8.calc(y);
    output += y;

    output *= 0.25f; // normalize output

    return join(output, input, wet_);
}
