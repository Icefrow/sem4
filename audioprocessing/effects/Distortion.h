#pragma once

class Distortion
{
public:
	explicit Distortion()
	{

	}

	void set(float power, float effect, float offset, float gisterezis, float currentState);

	float calcRoot(float input, float power);

	float overdrive(float input);

	float heavy(float input);

	float shmitt(float x);

private:
    float heavyLog(float input);

    float power_;
	float effect_;
	float offset_;
	float K_;
	float gisterezis_;
	float currentState_;
};