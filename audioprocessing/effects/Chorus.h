#pragma once
#include <dsp/ModulatedDelay.h>

class Chorus // 1 voice
{
public:
    Chorus(float maxDelay = 0.025f)
        : voice(maxDelay) {
        set(1, 4, 0.4f);
    }

    void set(float depth, float speed, float wet);

    float calc(float input);

private:
    ModulatedDelay voice;
    float depth_;
    float currentDelay_;
    float speed_;
    float wet_;
    bool up_ = false;
};
