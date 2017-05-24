#pragma once


/* ------------------------------------------ */
class LimiterByDividing
{
public:
    LimiterByDividing() {}

    LimiterByDividing(float power, float offset) {
        set(power, offset);
    }

    void set(float power, float offset) {
        power_ = power;
        offsetX_ = offset;
        offsetY_ = limit(offsetX_);
    }

    float calc(float input) const {
        input = (input - offsetX_) * power_;
        return (input >= 0 ? limit(input) : -limit(-input)) + offsetY_;
    }

private:
    static float limit(float x) {
        return 1 - (1 / (1 + x));
    }

    float power_ = 10;
    float offsetX_ = 0.0001f;
    float offsetY_ = limit(offsetX_);
};

/* ------------------------------------------ */
class RootLimiter
{
public:
    explicit RootLimiter(float power = 0.5f, float offset = 0.002f) {
        set(power, offset);
    }

    void set(float power, float offset) {
        power_ = 1 / (1 + power * 4);
        offsetX_ = offset;
        offsetY_ = offsetX_ >= 0 ? limit(offsetX_) : -limit(-offsetX_);
    }

    float calc(float input) const {
        input = input - offsetX_;
        return (input >= 0 ? limit(input) : -limit(-input)) + offsetY_;
    }

private:
    float limit(float x) const {
        return pow(x, power_);
    }

    float power_;
    float offsetX_;
    float offsetY_;
};




