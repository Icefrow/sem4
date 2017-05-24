#pragma once

#include <string>
#include <util/AudioData.h>

void readWAV(const uint8_t * mem, size_t size, AudioData & outData);

void readWAV(const std::string & fileName, AudioData & outData);

void readWAV(HMODULE dllOrExe, const char * resourceName, AudioData & outData);

