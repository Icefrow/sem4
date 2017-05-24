#include "pch.h"

extern float const PI = 3.1415926535897932384626433832795f;
extern float const f2_PI = 2 * PI;

namespace Dsp {

unsigned SampleRate = 48000;
float Period = 1.f / SampleRate;

void setSampleRate(unsigned sampleRate)
{
    SampleRate = sampleRate;
    Period = 1.f / SampleRate;
}

} // namespace Globals