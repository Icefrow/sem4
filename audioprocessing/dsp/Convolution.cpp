#include "pch.h"

#include <dsp/Convolution.h>
#include <dsp/Globals.h>
#include <util/WAVReader.h>
#include <util/AudioConverter.h>

static const unsigned MaximumPossibleCount = 16000;

void Convolution::prepare()
{
    if(data_.sampleRate != Dsp::SampleRate) {
        AudioData data;
        const HINSTANCE hInstance = GetModuleHandle(NULL);
        readWAV(hInstance, "CABINET.WAV", data);

        if(data.sampleRate == Dsp::SampleRate)
            data.swap(data_);
        else
            resample(data, data_, Dsp::SampleRate);
    }

    delayLine_.resize(0);
    delayLine_.resize(data_.sampleCount + 4);

    unsigned maxCount = data_.sampleCount / 4 * 4;

    if(maxCount > MaximumPossibleCount)
        maxCount = MaximumPossibleCount;

    double result = 0;

    float * data = data_.data[0];

    for(unsigned i = 0; i < maxCount; i++)
        result += data[i];

    weight_ = float(result * maxCount / 64);

    index_ = 0;
}

float Convolution::calc(float input)
{
    delayLine_[index_] = input;

    if(index_ < 4)
        delayLine_[index_ + data_.sampleCount] = input;

    unsigned count = data_.sampleCount;
    unsigned maxCount = count / 4 * 4;

    if(maxCount > MaximumPossibleCount)
        maxCount = MaximumPossibleCount;

    const float * data = data_.data[0]; // left channel only for now
    const float * delayLine = &delayLine_[0];

    unsigned index = index_;

    float result = 0;

    for(unsigned i = 0; i < maxCount; i += 4) {
        result += delayLine[index] * data[i] +
                  delayLine[index + 1] * data[i + 1] +
                  delayLine[index + 2] * data[i + 2] +
                  delayLine[index + 3] * data[i + 3];

        index = (index + 4) % count;
    }

    index_ = (index_ + count - 1) % count;

    return result / weight_;
}
