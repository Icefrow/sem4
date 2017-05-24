#pragma once
#include <util/NonCopyable.h>
#include <util/CalcHelper.h>

/* ---------------------------------------------------------- */
class Device : NonCopyable
{
public:
    virtual ~Device();

    virtual void process(float * input, float * output, size_t size) = 0;

    virtual void devCtl(int controllerId, float value) = 0;
};

/* ---------------------------------------------------------- */
template<typename Effect>
class DeviceImpl : public Device
{
    void process(float * input, float * output, size_t size) override {
        while(size--)
            *output++ = effect_.calc(*input++);
    }

protected:
    Effect effect_;
};

