#pragma once
#include "Inductor.h"
#include "Capacitor.h"

/* ------------------------------------------------- */
class LowPassFilter1 
{
public:
    explicit LowPassFilter1(float f = 1000) {
        set(f);
    }

    void set(float f) {
        float R = 1;
        float C = 1 / (f * R);
        c_.capacity(C);
    }

    float calc(float x) {
        return c_.calc(x - c_.potential()); // R == 1;
    }

private:
    Capacitor c_;
};

/* ------------------------------------------------- */
class HighPassFilter1
{
public:
    explicit HighPassFilter1(float f = 1000) {
        set(f);
    }

    void set(float f) {
        float R = 1;
        float C = 1 / (f * R);
        c_.capacity(C);
    }

    float calc(float x) {
        float y = x - c_.potential();
        c_.calc(y);
        return y;
    }

private:
    float f_;
    Capacitor c_;
};

/* ------------------------------------------------- */
// http://sim.okawa-denshi.jp/images/RLCLowPass2.png
class LowPassFilter2
{
public:
    LowPassFilter2() {
        set(1000, 0.705f);
    }

    explicit LowPassFilter2(float f, float q) {
        set(f, q);
    }

    void set(float f, float q);

    float calc(float u);

private:
    Inductor l_;
    Capacitor c_;
};

/* ------------------------------------------------- */
class HighPassFilter2
{
public:
    explicit HighPassFilter2() {
        set(1000, 0.705f);
    }

    explicit HighPassFilter2(float f, float q) {
        set(f, q);
    }

    void set(float f, float q);

    float calc(float u);

private:
    Inductor l_;
    Capacitor c_;
    float r_;
};

/* ------------------------------------------------- */
class BandPassFilter2
{
public:
    explicit BandPassFilter2() {
        set(1000, 0.705f);
    }

    explicit BandPassFilter2(float f, float q) {
        set(f, q);
    }

    void set(float f, float q);

    float calc(float u);

private:
    LowPassFilter2 low_;
    HighPassFilter2 high_;
};


/* ------------------------------------------------- */
template<typename Filter, unsigned N>
class ResamplingFilter1
{
public:
    explicit ResamplingFilter1(float f = 1000)
        : filter_(f / N)
    {}

    void set(float f) {
        filter_.set(f);
    }

    float calc(float input) {
        for(unsigned i = 0; i < N - 1; i++)
            filter_.calc(input);

        return filter_.calc(input);
    }

private:
    Filter filter_;
};

/* ------------------------------------------------- */
template<typename Filter, unsigned N>
class ResamplingFilter2
{
public:
    explicit ResamplingFilter2(float f = 1000, float q = 0.7f)
        : filter_(f / N, q)
    {}

    void set(float f, float q) {
        filter_.set(f, q);
    }

    float calc(float input) {
        for(unsigned i = 0; i < N - 1; i++)
            filter_.calc(input);

        return filter_.calc(input);
    }

private:
    Filter filter_;
};

/* ------------------------------------------------- */
typedef ResamplingFilter1<LowPassFilter1, 4> Lpf1;
typedef ResamplingFilter1<HighPassFilter1, 4> Hpf1;

typedef ResamplingFilter2<LowPassFilter2, 4> Lpf2;
typedef ResamplingFilter2<HighPassFilter2, 4> Hpf2;
typedef ResamplingFilter2<BandPassFilter2, 4> Bpf2;
