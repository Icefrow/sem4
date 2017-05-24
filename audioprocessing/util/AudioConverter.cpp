#include "pch.h"

#include "AudioConverter.h"
#include "AudioData.h"
#include <memory>

/* -------------------------------------------------------------------- */
// converting
/* -------------------------------------------------------------------- */
namespace {

const double scale32 = double(0x7fffffffL) + .499999;
const double scale16 = double(0x7fffL) + .499999;

const double int32Max = INT32_MAX;
const double int32Min = INT32_MIN + 1;

const double int16Max = INT16_MAX;
const double int16Min = INT16_MIN + 1;

}   // namespace

/* -------------------------------------------------------------------- */
void float32toInt32inPlace(float * buffer, size_t frames)
{
    while(frames--) {
        double d = *buffer * scale32;

        if(d > int32Max) *reinterpret_cast<long *>(buffer) = INT32_MAX;
        else if(d < int32Min) *reinterpret_cast<long *>(buffer) = INT32_MIN;
        else *reinterpret_cast<long *>(buffer) = long(d);

        buffer++;
    }
}

/* -------------------------------------------------------------------- */
void int32toFloat32inPlace(long * buffer, size_t frames)
{
    while(frames--) {
        *reinterpret_cast<float *>(buffer) = float(*buffer / scale32);
        buffer++;
    }
}

/* -------------------------------------------------------------------- */
void int32toFloat32(const int32_t * src, float * dst, size_t frames)
{
    while(frames--)
        *dst++ = float(*src++ / scale32);
}

void int32toFloat32(const int32_t * stereoSrc, float * monoDst1, float * monoDst2,
                    size_t frames)
{
    while(frames--) {
        *monoDst1++ = float(*stereoSrc++ / scale32);
        *monoDst2++ = float(*stereoSrc++ / scale32);
    }
}

inline
int readInt24(const uint8_t * data)
{
    return data[0] << 8 | data[1] << 16 | data[2] << 24;
}

/* -------------------------------------------------------------------- */
void int24toFloat32(const uint8_t * src, float * dst, size_t frames)
{
    while(frames--) {
        *dst++ = float(readInt24(src) / scale32);
        src += 3;
    }
}

void int24toFloat32(const uint8_t * stereoSrc, float * monoDst1, float * monoDst2,
                    size_t frames)
{
    while(frames--) {
        *monoDst1++ = (float)(*stereoSrc++ / scale32);
        stereoSrc += 3;
        *monoDst2++ = (float)(*stereoSrc++ / scale32);
        stereoSrc += 3;
    }
}

/* -------------------------------------------------------------------- */
void int16toFloat32(const int16_t * src, float * dst, size_t frames)
{
    while(frames--)
        *dst++ = float(*src++ / scale16);
}

void int16toFloat32(const int16_t * stereoSrc, float * monoDst1, float * monoDst2,
                    size_t frames)
{
    while(frames--) {
        *monoDst1++ = float(*stereoSrc++ / scale16);
        *monoDst2++ = float(*stereoSrc++ / scale16);
    }
}

/* -------------------------------------------------------------------- */
void float32StereoTofloat32Mono(const float * stereoSrc, float * monoDst1,
                                float * monoDst2, size_t frames)
{
    while(frames--) {
        *monoDst1++ = *stereoSrc++;
        *monoDst2++ = *stereoSrc++;
    }
}

/* -------------------------------------------------------------------- */
// resampling
/* -------------------------------------------------------------------- */
namespace {

// add zeros at the end of src data if needed
float getValueOrZero(const float * src, unsigned index, unsigned size)
{
    if(index >= size)
        return 0;

    return src[index];
}

// resample by integer factor
void resampleUpK(const float * src, unsigned srcSamples, float * dst, unsigned K)
{
    for(unsigned i = 0; i < srcSamples; i++) {
        float value = src[i];

        for(unsigned j = i * K, e = j + K; j < e; j++)
            *dst++ = value;
    }
}

// resample by integer factor
void resampleDownK(const float * src, unsigned srcSamples, float * dst, unsigned K)
{
    float value = 0;
    float fK = 1.0f / K;

    for(unsigned i = 0; i < srcSamples; i += K) {
        for(unsigned j = i; j < i + K; j++)
            value += getValueOrZero(src, j, srcSamples);

        *dst++ = value * fK;
        value = 0;
    }
}

// resample up by fractional factor
void resampleUp(const float * src, unsigned srcSamples, unsigned srcRate,
                float * dst, unsigned dstSamples, unsigned dstRate)
{
    unsigned srcPhase = 0;

    float value = 0;
    unsigned srcIndex = 0;

    while(dstSamples--) {

        srcPhase += srcRate;

        if(srcPhase >= dstRate) {
            srcPhase -= dstRate;
            float a = float(srcRate - srcPhase) / srcRate;
            value += getValueOrZero(src, srcIndex, srcSamples) * a +
                     getValueOrZero(src, srcIndex + 1, srcSamples) * (1 - a);
            srcIndex++;
        }
        else
            value += getValueOrZero(src, srcIndex, srcSamples);

        *dst++ = value;
        value = 0;
    }
}

// resample down by fractional factor
void resampleDown(const float * src, unsigned srcSamples, unsigned srcRate,
                  float * dst, unsigned dstSamples, unsigned dstRate)
{
    // to resample down we firs resample up...
    // this gives better result after resampling
    unsigned K = (srcRate + dstRate - 1) / dstRate;

    unsigned tmpSamples = dstSamples * K;
    unsigned tmpRate = dstRate * K;
    std::unique_ptr<float[]> tmp(new float[tmpSamples]);

    resampleUp(src, srcSamples, srcRate, tmp.get(), tmpSamples, tmpRate);

    // after resampling up we calculate average values by integer factor K
    resampleDownK(tmp.get(), tmpSamples, dst, K);
}

void resampleImpl(const float * src, unsigned srcSamples, unsigned srcRate,
                  float * dst, unsigned dstSamples, unsigned dstRate)
{
    if(srcRate < dstRate) {
        unsigned K = dstRate / srcRate;

        if(srcRate * K == dstRate) {
            assert(srcSamples * K == dstSamples);
            resampleUpK(src, srcSamples, dst, K);
            return;
        }

        resampleUp(src, srcSamples, srcRate, dst, dstSamples, dstRate);
    }
    else {
        unsigned K = srcRate / dstRate;

        if(dstRate * K == srcRate) {
            resampleDownK(src, srcSamples, dst, K);
            return;
        }

        resampleDown(src, srcSamples, srcRate, dst, dstSamples, dstRate);
    }
}

} // namespace

// public function
void resample(const AudioData & src, AudioData & dst, unsigned dstRate)
{
    size_t dstSamples = (int64_t(src.sampleCount) * dstRate + src.sampleRate - 1)
                        / src.sampleRate;

    if(src.isStereo()) {
        dst.createStereo(dstSamples, dstRate);

        resampleImpl(src.data[0], src.sampleCount, src.sampleRate,
                     dst.data[0], dst.sampleCount, dstRate);

        resampleImpl(src.data[1], src.sampleCount, src.sampleRate,
                     dst.data[1], dst.sampleCount, dstRate);
    }
    else {
        dst.createMono(dstSamples, dstRate);

        resampleImpl(src.data[0], src.sampleCount, src.sampleRate,
                     dst.data[0], dst.sampleCount, dstRate);
    }
}
