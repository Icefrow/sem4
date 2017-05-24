#pragma once

#include <dsp/Capacitor.h>

class AttackRelease
{
public:
    AttackRelease(float attackInSec, float releaseInSec);

    void set(float attackInSec, float releaseInSec);

    float calc(float input);

private:
    Capacitor c_;
    float rAttackReciprocal_;
};

