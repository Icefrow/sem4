#include "pch.h"

#include <dsp/SineGenerator.h>

SineGenerator::SineGenerator(float freq)
    : phase_()
{
    set(freq);
}

void SineGenerator::set(float freq)
{
    freq_ = freq;
    prepare();
}

void SineGenerator::prepare() 
{
    step_ = f2_PI * freq_ / Dsp::SampleRate;
}

ModulatedSineGenerator::ModulatedSineGenerator()
    : phase_()
{}
