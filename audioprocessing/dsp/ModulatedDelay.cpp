#include "pch.h"
#include "ModulatedDelay.h"
#include <dsp/Globals.h>

ModulatedDelay::ModulatedDelay(float maxDelayIsSec)
    : writeIndex_(0)
    , size_(0)
    , maxDelayInSec_(maxDelayIsSec)
{}

void ModulatedDelay::prepare()
{
    size_ = unsigned(Dsp::SampleRate * maxDelayInSec_) + 1;
    buffer_.resize(0);
    buffer_.resize(size_);
}

float ModulatedDelay::calc(float input, float currentDelay)
{
    unsigned size = size_;
    unsigned writeIndex = writeIndex_;

    buffer_[writeIndex] = input;

    float readIndex = writeIndex - currentDelay * Dsp::SampleRate;
    writeIndex_ = (writeIndex + 1) % size;

    if(readIndex < 0)
        readIndex += size;

    unsigned readIndex0 = int(readIndex) % size;
    unsigned readIndex1 = (readIndex0 + 1) % size;
    float fraction = readIndex - floor(readIndex);

    return buffer_[readIndex0] * (1 - fraction) + buffer_[readIndex1] * fraction;
}
