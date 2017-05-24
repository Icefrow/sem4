#pragma once

#include <functional>
#include "AudioProcessor.h"

// Forward declaration
interface IASIO;

namespace Asio {

using namespace std;

/* ---------------------------------------------------- */
// Wrapper around ASIO driver

class Driver
{
public:
    explicit Driver(const CLSID & clsid, HWND hwnd);

    ~Driver();

    const CLSID & clsid() const {
        return clsid_;
    }

    void readInfo(DriverInfo & result) const;

    void showControlPanel();

    double sampleRate() const {
        return sampleRate_;
    }

    void setSampleRate(double sampleRate);

    void subscribeOnDriverWantsReset(function<void()> callback) {
        driverWantsResetCallback_ = callback;
    }

    void createBuffers(int inputChan, int outChanL, int outChanR, long bufferSize);

    void start();
    void stop();

private:
    static long asioMessageCallBack(long selector, long value, void * message, double * opt);
    static void asioSampleRateDidChangeCallback(double sRate);
    static void bufferSwitch(long doubleBufferIndex, long directProcess);

    static Driver * currentDriver_;
    double sampleRate_;

    IASIO * asio_;
    bool started_ = false;
    CLSID clsid_;
    string name_;
    long version_;

    function<void()> driverWantsResetCallback_;
};

}   // namespace Asio




