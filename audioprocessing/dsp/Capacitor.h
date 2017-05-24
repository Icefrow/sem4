#pragma once
#include <util/Preparable.h>

class Capacitor : protected Preparable
{
public:
    explicit Capacitor(float capacityArg = 0.0000001f);

    float capacity() const {
        return capacity_;
    }

    void capacity(float newCapacity);

    float potential() const {
        return float(potential_);
    }

    // returns potencial
    float calc(float i) {
        return float(potential_ += i * k_);
    }

    void prepare() override;

private:
    double potential_;
    double volatile k_;
    float capacity_;
};

