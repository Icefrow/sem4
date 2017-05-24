#pragma once

#include <dsp/AttackRelease.h>

class AmplitudeDetector : public AttackRelease
{
    typedef AttackRelease base;

public:
	explicit AmplitudeDetector(float attackInSec = 0.015f, float releaseInSec = 0.500f);

    float calc(float input);
};
