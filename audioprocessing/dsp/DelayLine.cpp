#include "pch.h"

#include <dsp/DelayLine.h>
#include <dsp/Globals.h>

DelayLine::DelayLine(float delay)
    : delay_(delay)
{}

float DelayLine::calc(float input)
{
    float output = buffer_[index_];
    buffer_[index_] = input;
    index_ = (index_ + 1) % size_;
    return output;
}

void DelayLine::prepare()
{
    size_ = int(Dsp::SampleRate * delay_); // sec
    buffer_.resize(0);
    buffer_.resize(size_);
}
