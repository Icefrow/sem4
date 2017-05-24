#pragma once
#include <util/Preparable.h>

class Inductor : protected Preparable
{
public:
    explicit Inductor(float inductanceArg = 0.001);
    
    float inductance() const {
        return inductance_;
    }

    void inductance(float newInductance);

    float current() const {
        return float(current_);
    }

    // returns current
    float calc(float u) {
        return float(current_ += u * k_);
    }

    void prepare() override;

private:
    double current_;
    double volatile k_;
    float inductance_;
};

