#pragma once

struct AudioData;

/* --------------------------------------------------------------------- */
// Data format conversions
void float32toInt32inPlace(float * buffer, size_t frames);

void int32toFloat32inPlace(long * buffer, size_t frames);

void int32toFloat32(const int32_t * src, float * dst, size_t frames);

void int32toFloat32(const int32_t * stereoSrc, float * monoDst1, float * monoDst2, size_t frames);

void int24toFloat32(const uint8_t * src, float * dst, size_t frames);

void int24toFloat32(const uint8_t * stereoSrc, float * monoDst1, float * monoDst2, size_t frames);

void int16toFloat32(const int16_t * src, float * dst, size_t frames);

void int16toFloat32(const int16_t * stereoSrc, float * monoDst1, float * monoDst2, size_t frames);

void float32StereoTofloat32Mono(const float * stereoSrc, float * monoDst1, float * monoDst2, size_t frames);

/* --------------------------------------------------------------------- */
// Sample rate conversions
void resample(const AudioData & src, AudioData & dst, unsigned dstRate);

/* --------------------------------------------------------------------- */
// Value conversions
inline
float limitValue(float value, float minValue, float maxValue)
{
    if (value < minValue)
        return minValue;

    if (value > maxValue)
        return maxValue;

    return value;
}