#include "pch.h"

#include <dsp/Globals.h>
#include <dsp/Inductor.h>

Inductor::Inductor(float inductanceArg)
    : current_(0)
{
    inductance(inductanceArg);
}

void Inductor::inductance(float newInductance)
{
    inductance_ = newInductance;
    prepare();
}

void Inductor::prepare()
{
    k_ = Dsp::Period / double(inductance_);
}
