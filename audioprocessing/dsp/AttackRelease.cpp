#include "pch.h"
#include "AttackRelease.h"

AttackRelease::AttackRelease(float attackInSec, float releaseInSec)
{
    set(attackInSec, releaseInSec);
}

void AttackRelease::set(float attackInSec, float releaseInSec)
{
    // Tau = R*C, transition process is ~3*Tau
    float R_release = 1;
    float C = releaseInSec / (3 * R_release);
    float R_attack = attackInSec / (3 * C);

    c_.capacity(C);
    rAttackReciprocal_ = 1 / R_attack;
}

float AttackRelease::calc(float input)
{
    float u_delta = input - c_.potential();

    if(u_delta > 0)
        return c_.calc(u_delta * rAttackReciprocal_);

    return c_.calc(u_delta);
}

