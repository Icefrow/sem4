#pragma once

#include <vector>

class Device;

/* ---------------------------------------------------- */
// Driver info
struct DriverId {
    CLSID clsid;
    std::string name;
};

enum {
    CMD_RESET_DEVICE = 2048 // will post message to supplied HWND handle
};

/* ---------------------------------------------------- */
// Audio channel info
struct ChannelInfo {
    bool isActive;      // on exit
    long channelGroup;  // dto
    long type;          // dto
    std::string name;        // dto
};

/* ---------------------------------------------------- */
// Audio buffer info
struct BufferInfo {
    long minSize;
    long maxSize;
    long preferredSize;
    long granularity;
};

/* ---------------------------------------------------- */
struct Latencies {
    int input;
    int output;
};

/* ---------------------------------------------------- */
// Driver information
struct DriverInfo {
    CLSID clsid;
    std::vector<ChannelInfo> inputChannels;
    std::vector<ChannelInfo> outputChannels;
    std::vector<double> supportedSampleRates;
    Latencies latencies;
    BufferInfo bufferInfo;
};

/* ---------------------------------------------------- */
// Audio parameters for starting driver
struct AudioParams {
    int inputChannel;       // index of input channel
    int outputChannelL;     // index of output channel L 
    int outputChannelR;     // R
    int bufferSize;
};

/* ---------------------------------------------------- */
// Core audio engine
class AudioProcessor
{
public:
    /* --------------------------------------------------- */
    // driver stuff

    // Initialization routine.
    // Loads config from exe-file-name.cfg
    // returns true if config has loaded and succssfully applied
    static bool init(HWND hwnd);

    // saves curen config
    static void saveConfig();

    // reads divers' info from registry
    static void getDriverList(std::vector<DriverId> & result);

    // stops current driver and loads new one
    static void openDriver(const CLSID & driverClsId, bool defaultParams = true);

    // stops and closes current driver
    static void closeDriver();

    // info about curent driver
    static const DriverInfo & driverInfo();

    // channel/buffer parameters of current driver
    static const AudioParams & audioParams();

    // start/restart driver with new params
    static void changeParams(const AudioParams & params);

    static double sampleRate();

    static void setSampleRate(double sampleRate);

    // true if driver was started already
    static bool isRunning();

    static void showControlPanel();

    /* --------------------------------------------------- */
    // audio processing stuff

    // add device to chain
    static void addDevice(Device * device, int index);

    // remove device from chain
    static void removeDevice(int index);

    // set device controller value
    static void deviceCtl(Device * device, int ctl, float value);

private:
    AudioProcessor() = delete;
    ~AudioProcessor() = delete;
};

