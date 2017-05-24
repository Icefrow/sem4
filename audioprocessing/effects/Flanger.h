#pragma once
#include "dsp/ModulatedDelay.h"
#include "dsp/SineGenerator.h"

class Flanger
{
public:
    explicit Flanger(float delayInSec = 0.006f, float deep = 0.5f, float freq = 0.5f, float wet = 0.33f);

    void set(float delayInSec = 0.006f, float deep = 0.5f, float freq = 0.5f, float wet = 0.33f);

    float calc(float input);

    static constexpr float MinDelay = 0.003f;   // 3 ms
    static constexpr float MaxDelay = 0.025f;   // 25 ms

    ModulatedDelay delayLine_;
    float delay_;
    float modulationAmplitude_;
    SineGenerator generator_;
    float wet_ = 0.33f;
	float deep_ = 0.5f;
	float freq_ = 0.5f;
	float delayInSec_ = 0.006f;
};

