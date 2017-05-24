#pragma once
#include <vector>
#include <util/Preparable.h>

class DelayLine : protected Preparable
{
public:
    explicit DelayLine(float delay = 0.475f);

    float current() const {
        return buffer_[index_];
    }

    float calc(float input);

    void prepare() override;

private:
    std::vector<float> buffer_;
    unsigned size_ = 0;
    unsigned index_ = 0;
    float delay_;
};

