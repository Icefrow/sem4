#pragma once

#include <vector>
#include <util/AudioData.h>
#include <util/Preparable.h>

class Convolution : protected Preparable
{
public:
    float calc(float input);

    void prepare() override;

private:
    AudioData data_;
    std::vector<float> delayLine_;
    unsigned index_ = 0;
    float weight_ = 0;
};

