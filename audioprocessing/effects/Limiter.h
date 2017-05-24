#pragma once

#include "AmplitudeDetector.h"

class Limiter
{
public:
	Limiter()
	{
		set(1.f);
	}

	void set(float gain);

	float calc(float input);

private:
	float gain_;
	AmplitudeDetector amplitude_;
};