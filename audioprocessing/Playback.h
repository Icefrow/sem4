#pragma once

#include <util/AudioData.h>

class Playback : protected Preparable
{
public:
    float volume() const {
        return volume_;
    }

    void volume(float value) {
        volume_ = value;
    }

    void prepare() override;

    void addTo(float * outL, float * outR, size_t count);

private:
    AudioData sound_;
    float volatile volume_ = 1;
    size_t index_ = 0;
};

