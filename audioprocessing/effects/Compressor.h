#pragma once
#include "AmplitudeDetector.h"

struct Compressor
{

	float calc(float input);

	void set(float attack, float release);

	float ratio_ = 2;
	float threshold_ = 0.5;
	float gain_ = 1;
	float attack_ = 1;
	float release_ = 0.2;
	AmplitudeDetector amplitude_ ;

};