#include "pch.h"
#include "Distortion.h"

void Distortion::set(float power, float effect, float offset, float gisterezis, float currentState)
{
	power_ = power;
	effect_ = effect;
	offset_ = offset;
	K_ = 0.15f / log(effect);
	gisterezis_ = gisterezis;
	currentState_ = currentState;
}

float Distortion::calcRoot(float x, float power)
{
	return x >= 0 ? pow(x, power) : -pow(-x, power);
}

float Distortion::overdrive(float x)
{
	return calcRoot(x + 0.004f, 2.0f / 3) * 0.8f + calcRoot(x - 0.004f, 2.0f / 3) * 0.8f;
}

float Distortion::heavyLog(float x)
{
    x = x * effect_;

    if (x < 0)
        return -log(1 - x) * K_;

    float y = log(1 + x) * K_;
    return y;
}

float Distortion::heavy(float x)
{
    float y = heavyLog(x + offset_) + heavyLog(x - offset_);
    return y;
}

float Distortion::shmitt(float x)
{
	currentState_ = (currentState_ * gisterezis_ + x - gisterezis_) > 0 ? 1.0f : 0;
	return currentState_ / 2;
}
