#include "pch.h"
#include "AmplitudeDetector.h"

AmplitudeDetector::AmplitudeDetector(float attackInSec, float releaseInSec)
    : base(attackInSec, releaseInSec) {}

float AmplitudeDetector::calc(float input)
{
    return base::calc(abs(input));
}
