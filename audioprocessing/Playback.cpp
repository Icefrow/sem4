#include "pch.h"

#include <util/AudioConverter.h>
#include <util/WAVReader.h>
#include <dsp/SineGenerator.h>

#include "Playback.h"

void Playback::prepare()
{
    if(sound_.sampleRate != Dsp::SampleRate) {
        AudioData sound;
        const HINSTANCE hInstance = GetModuleHandle(NULL);
        readWAV(hInstance, "PLAYBACK.WAV", sound);

        if(sound.sampleRate == Dsp::SampleRate)
            sound.swap(sound_);
        else
            resample(sound, sound_, Dsp::SampleRate);
    }

    index_ = 0;
}

void Playback::addTo(float * outL, float * outR, size_t count)
{
    if(size_t sampleCount = sound_.sampleCount) {
        float * dataL = sound_.data[0];
        float * dataR = sound_.data[1];
        float vol = volume();

        while(count--) {
            *outL++ += dataL[index_] * vol;
            *outR++ += dataR[index_] * vol;

            index_ = (index_ + 1) % sampleCount;
        }
    }
}
