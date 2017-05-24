#include "pch.h"
#include "Compressor.h"

void Compressor::set(float attack, float release)
{
    amplitude_.set(attack, release);
}

float Compressor::calc(float input)
{
    float ampl = amplitude_.calc(input);

    if(ampl > threshold_)
        return gain_ * input * (threshold_ + (ampl - threshold_) / ratio_) / ampl;

    return input * gain_; 
}