#include "pch.h"

#include <dsp/Globals.h>
#include <dsp/Capacitor.h>

Capacitor::Capacitor(float capacityArg)
    : potential_(0)
{
    capacity(capacityArg);
}

void Capacitor::capacity(float newCapacity) {
    capacity_ = newCapacity;
    prepare();
}

void Capacitor::prepare() {
    k_ = Dsp::Period / double(capacity_);
}
