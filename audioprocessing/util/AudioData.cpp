#include "pch.h"

#include <util/AudioData.h>

AudioData::AudioData()
    : sampleCount()
    , sampleRate()
{
    data[0] = data[1] = nullptr;
}

AudioData::~AudioData()
{
    destroy();
}

void AudioData::createMono(size_t samples, uint32_t rate)
{
    destroy();

    size_t memSize = samples * sizeof(float);
    data[0] = reinterpret_cast<float *>(malloc(memSize));

    if(!data[0])
        throw std::bad_alloc();

    data[1] = data[0];
    sampleCount = samples;
    sampleRate = rate;
}

void AudioData::createStereo(size_t samples, uint32_t rate)
{
    destroy();

    size_t memSize = samples * sizeof(float);
    data[0] = reinterpret_cast<float *>(malloc(memSize));
    data[1] = reinterpret_cast<float *>(malloc(memSize));

    if(!data[0] || !data[1])
        throw std::bad_alloc();

    sampleCount = samples;
    sampleRate = rate;
}

void AudioData::destroy()
{
    if(data[0])
        free(data[0]);

    if(data[1] && data[1] != data[0])
        free(data[1]);

    data[0] = data[1] = 0;
    sampleCount = 0;
}

void AudioData::swap(AudioData & other) noexcept
{
    std::swap(data[0], other.data[0]);
    std::swap(data[1], other.data[1]);
    std::swap(sampleCount, other.sampleCount);
    std::swap(sampleRate, other.sampleRate);
}

