#pragma once

#include <util/NonCopyable.h>

struct AudioData : public NonCopyable {
    AudioData();

    ~AudioData();

    bool isStereo() const {
        return data[0] != data[1];
    }

    void createMono(size_t samples, uint32_t rate);

    void createStereo(size_t samples, uint32_t rate);

    void destroy();

    void swap(AudioData & other) noexcept;

    float * data[2];    // 1 or 2 channels
    unsigned sampleCount;
    unsigned sampleRate;
};

