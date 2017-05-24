#include "pch.h"
#include "Chorus.h"

static int Frequncy = 96000;
static float Period = 1.f / Frequncy;

void Chorus::set(float depth, float speed, float wet)
{
//	voice.set(depth, 0, 1);
	depth_ = depth;
	currentDelay_ = depth_;
	speed_ = speed;
	wet_ = wet;
}

float Chorus::calc(float input)
{
	float input2 = voice.calc(input);

	if (currentDelay_ >= 0.0001 || up_ == false)
		currentDelay_ -= speed_ * Period;
	else if (currentDelay_ < 0.0001 || up_ == false)
		up_ = true;
	else if (currentDelay_ <= depth_ || up_ == true)
		currentDelay_ += speed_ * Period;
	else if (currentDelay_ > depth_ || up_ == true)
		up_ = false;

	voice.change(currentDelay_);
	return input*(1 - wet_) + input2 * wet_;
}