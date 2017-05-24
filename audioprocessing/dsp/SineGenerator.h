#pragma once

#include <dsp/Globals.h>
#include <util/Preparable.h>

/* ------------------------------------------------- */
class SineGenerator : protected Preparable
{
public:
    explicit SineGenerator(float freq = 1000);

    void set(float freq);

    float calc() {
        float ph = phase_ + step_;

        if(ph > f2_PI)
            ph -= f2_PI;

        phase_ = ph;
        return sin(ph);
    }

    void prepare() override;

private:
    float phase_;
    float volatile step_;
    float freq_;
};

/* ------------------------------------------------- */
class ModulatedSineGenerator
{
public:
    explicit ModulatedSineGenerator();

    float calc(float freq) {
        float step = f2_PI * freq / Dsp::Period;
        float ph = phase_ + step;

        if(ph > f2_PI)
            ph -= f2_PI;

        phase_ = ph;
        return sin(ph);
    }

private:
    float phase_;
};

