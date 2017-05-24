#pragma once
#include <vector>
#include <util/Preparable.h>

class ModulatedDelay : protected Preparable
{
public:
    explicit ModulatedDelay(float maxDelayIsSec);

	float calc(float input, float currentDelay);

protected:
    void prepare() override;

private:
    unsigned writeIndex_;
    int size_;
	std::vector<float> buffer_;
    float maxDelayInSec_;
};
