#include "pch.h"

#include <boost/property_tree/ini_parser.hpp>
#include <memory>
#include <asio/asiolist.h>

#include "util/AsioHelper.h"
#include "AudioProcessor.h"

#include <dsp/Globals.h>
#include <dsp/Filters.h>

#include "Playback.h"
#include <effects/Screamer.h>
#include <effects/NoiseGate.h>
#include <effects/AmpEmulator.h>
#include <effects/Flanger.h>
#include <effects/Delay.h>
#include <dsp/Convolution.h>
#include <effects/Reverberator.h>
#include <util/CalcHelper.h>
#include "util/GuidHelper.h"
#include "ui/framework/Environment.h"
#include <atomic>
#include "devices/Device.h"

using namespace std;

namespace {
unique_ptr<Asio::Driver> driver_;
DriverInfo driverInfo_;
AudioParams audioParams_;
bool volatile isRunning_ = false;
HWND hwnd_ = nullptr;

vector<Device *> devices_;

struct Command {
    enum {
        ADD_DEVICE,
        REMOVE_DEVICE,
        DEVICE_CTL
    };

    int cmd;
    Device * device;
    int index;
    float value;

    Command * volatile next_;
};

atomic<Command *> commands_;

void enqueueCommand(Command * cmd)
{
    Command * head = commands_;

    while(true) {
        cmd->next_ = head;

        if(commands_.compare_exchange_weak(head, cmd))
            return;
    }
}

string configFileName()
{
    string fileName = Environment::programFullPath();
    size_t lastindex = fileName.find_last_of(".");
    return fileName.substr(0, lastindex) + ".cfg";
}

int getChannelIndex(const vector<ChannelInfo> & channels, const string & channelName)
{
    for(int i = 0, size = channels.size(); i < size; i++) {
        if(channels[i].name == channelName)
            return i;
    }

    throw runtime_error("Channel \"" + channelName + "\" not found");
}

void addDeviceImpl(Device * device, int index)
{
    devices_.insert(devices_.begin() + index, device);
}

void removeDeviceImpl(int index)
{
    delete devices_[index];
    devices_.erase(devices_.begin() + index);
}

void executeCommand(Command * cmd)
{
    switch(cmd->cmd) {
    case Command::ADD_DEVICE:
        addDeviceImpl(cmd->device, cmd->index);
        break;

    case Command::REMOVE_DEVICE:
        removeDeviceImpl(cmd->index);
        break;

    case Command::DEVICE_CTL:
        cmd->device->devCtl(cmd->index, cmd->value);
        break;
    }
}

Command * reverseLinkedList(Command * head)
{
    Command * nextNode = head->next_;
    head->next_ = nullptr;

    while(nextNode) {
        Command * nextNextNode = nextNode->next_;
        nextNode->next_ = head;
        head = nextNode;
        nextNode = nextNextNode;
    }

    return head;
}

void executeCommands()
{
    if (!commands_.load(memory_order_relaxed))
        return;

    Command * walker = commands_.exchange(nullptr);

    walker = reverseLinkedList(walker);

    do {
        executeCommand(walker);
        walker = walker->next_;
    }
    while(walker);
}

}   // namespace

void AudioProcessor::addDevice(Device * device, int index)
{
    if(!isRunning_) {
        executeCommands();
        addDeviceImpl(device, index);
        return;
    }

    Command * cmd = new Command{Command::ADD_DEVICE, device, index};
    enqueueCommand(cmd);
}

void AudioProcessor::removeDevice(int index)
{
    if(!isRunning_) {
        executeCommands();
        removeDeviceImpl(index);
        return;
    }

    Command * cmd = new Command{Command::REMOVE_DEVICE, nullptr, index};
    enqueueCommand(cmd);
}

void AudioProcessor::deviceCtl(Device * device, int ctl, float value)
{
    if(!isRunning_) {
        executeCommands();
        device->devCtl(ctl, value);
        return;
    }

    Command * cmd = new Command{Command::DEVICE_CTL, device, ctl, value};
    enqueueCommand(cmd);
}

void AudioProcessor::getDriverList(vector<DriverId> & result)
{
    const AsioDriverList driverList;

    auto driverInfo = driverList.lpdrvlist;
    size_t numdrv = driverList.numdrv;

    result.clear();
    result.reserve(numdrv);

    while(driverInfo) {
            result.push_back({ driverInfo->clsid, driverInfo->drvname });
        driverInfo = driverInfo->next;
    }
}

bool AudioProcessor::isRunning()
{
    return isRunning_;
}

void AudioProcessor::showControlPanel()
{
    assert(driver_.get());
    driver_->showControlPanel();
}

bool AudioProcessor::init(HWND hwnd)
{
    hwnd_ = hwnd;

    // try to read config

    namespace pt = boost::property_tree;
    bool ok = true;

    try {
        pt::ptree config;

        try {
            read_ini(configFileName(), config);
        }
        catch(const exception & ex) {
            throw runtime_error(string("Error during read ini file: ") + ex.what());
        }

        AudioParams params;

        string driverClsId = config.get<string>("Driver");
        double sampleRate = config.get<double>("Rate");
        params.bufferSize = config.get<int>("BufferSize");
        string inputChannel = config.get<string>("Input");
        string outputChannelL = config.get<string>("OutputL");
        string outputChannelR = config.get<string>("OutputR");

        cout << "Config loaded:" << endl;
        write_ini(cout, config);

        CLSID clsId;
        stringToGuid(driverClsId, clsId);

        //loadDriver(clsId, false);

        driverInfo_ = DriverInfo();
        driverInfo_.clsid = clsId;

        driver_.reset(new Asio::Driver(clsId, hwnd_));

        driver_->subscribeOnDriverWantsReset([] {
            PostMessage(hwnd_, WM_COMMAND, CMD_RESET_DEVICE, 0);
        });

        driver_->setSampleRate(sampleRate);
        driver_->readInfo(driverInfo_);

        try {
            params.inputChannel = getChannelIndex(driverInfo_.inputChannels, inputChannel);
        }
        catch(const exception & ex) {
            cout << ex.what() << endl;
            cout << "Channel " << driverInfo_.inputChannels[0].name << " will be used for input";
            params.inputChannel = 0;
            ok = false;
        }

        try {
            params.outputChannelL = getChannelIndex(driverInfo_.outputChannels, outputChannelL);
            params.outputChannelR = getChannelIndex(driverInfo_.outputChannels, outputChannelR);
        }
        catch(const exception & ex) {
            cout << ex.what() << endl;
            cout << "Channels "
                 << driverInfo_.outputChannels[0].name
                 << " and "
                 << driverInfo_.outputChannels[1].name
                 << " will be used for output";

            params.outputChannelL = 0;
            params.outputChannelR = 1;
            ok = false;
        }

        changeParams(params);
        return ok;
    }
    catch(const exception & ex) {
        cout << ex.what() << endl;

        const AsioDriverList driverList;

        auto firstDriver = driverList.lpdrvlist;

        if(firstDriver)
            openDriver(firstDriver->clsid);

        return false;
    }
}

void AudioProcessor::closeDriver()
{
    if(isRunning()) {
        driver_->stop();
        Sleep(50);

        isRunning_ = false;
        executeCommands();
    }

    driver_.reset();
    driverInfo_ = DriverInfo();
    audioParams_ = AudioParams();
}

void AudioProcessor::openDriver(const CLSID & driverClsId, bool defaultParams)
{
    closeDriver();

    driverInfo_ = DriverInfo();
    driverInfo_.clsid = driverClsId;

    driver_.reset(new Asio::Driver(driverClsId, hwnd_));

    driver_->subscribeOnDriverWantsReset([] {
        PostMessage(hwnd_, WM_COMMAND, CMD_RESET_DEVICE, 0);
    });

    driver_->readInfo(driverInfo_);

    if(defaultParams) {
        AudioParams params = {};
        params.outputChannelR = 1;
        params.bufferSize = driverInfo_.bufferInfo.preferredSize;
        changeParams(params);
    }
}

const DriverInfo & AudioProcessor::driverInfo()
{
    return driverInfo_;
}

const AudioParams & AudioProcessor::audioParams()
{
    return audioParams_;
}

double AudioProcessor::sampleRate()
{
    if(driver_)
        return driver_->sampleRate();

    return 0;
}

void AudioProcessor::setSampleRate(double sampleRate)
{
    if(driver_)
        driver_->setSampleRate(sampleRate);
}

void AudioProcessor::changeParams(const AudioParams & params)
{
    assert(driver_);

    if(isRunning_) {
        if(params.inputChannel != audioParams_.inputChannel ||
           params.outputChannelL != audioParams_.outputChannelL ||
           params.outputChannelR != audioParams_.outputChannelR ||
           params.bufferSize != audioParams_.bufferSize) {
            driver_->stop();
            isRunning_ = false;
        }
    }

    if(!isRunning_) {
        try {
            driver_->createBuffers(params.inputChannel,
                                   params.outputChannelL,
                                   params.outputChannelR,
                                   params.bufferSize);
        }
        catch(const exception & ex) {
            cout << "Error: Can not create buffers: " << ex.what() << endl;

            CLSID clsid = driver_->clsid();
            openDriver(clsid);

            driver_->createBuffers(params.inputChannel,
                                   params.outputChannelL,
                                   params.outputChannelR,
                                   params.bufferSize);
        }

        audioParams_ = params;  // save current params
        Dsp::setSampleRate((unsigned)driver_->sampleRate());

        Preparable::prepareAll();
        driver_->start();
        isRunning_ = true;
    }
}

void AudioProcessor::saveConfig()
{
    if(!driver_)
        throw logic_error("Driver is not initialized");

    if(!isRunning())
        throw logic_error("Driver was not started");

    namespace pt = boost::property_tree;

    pt::ptree config;

    config.put("Driver", driver_->clsid());
    config.put("Rate", driver_->sampleRate());
    config.put("BufferSize", audioParams_.bufferSize);
    config.put("Input", driverInfo_.inputChannels[audioParams_.inputChannel].name);
    config.put("OutputL", driverInfo_.outputChannels[audioParams_.outputChannelL].name);
    config.put("OutputR", driverInfo_.outputChannels[audioParams_.outputChannelR].name);

    write_ini(configFileName(), config);

    cout << endl;
    cout << "Config saved:" << endl;
    write_ini(cout, config);
}

void proccessAudio(float * input, float * outL, float * outR, long samples)
{
    executeCommands();

    for(Device * device : devices_)
        device->process(input, input, samples);

    copy(input, input + samples, outL);
    copy(input, input + samples, outR);

    //processor_.proccessChunk(input, outL, outR, samples);
    //playback.addTo(outL, outR, samples);
}

// ================================================================
struct AudioProcessorOld {

    NoiseGate noiseGate;
    Screamer screamer;
    AmpEmulator amplifier;
    Convolution cabinet;
    Flanger flanger{ 0.008f, 0.33f, 0.33f };
    Reverberator reverberator;
    Delay delay1;
    Delay delay2 { 0.275f, 0.44f };

    struct {
        Hpf1 flt1{ 250 };
        Hpf2 flt2{ 86, 1 };
        Lpf2 flt3{ 4500, 1.0f };

        float calc(float input) {
            float y = calcAll(input, flt1, flt2);
            return join(y, flt3.calc(input));
        }

    } cabinetReplacer;

    void proccess(float input, float * outL, float * outR) {

        float y = calcAll(input
                          , noiseGate
                          , screamer
                          , amplifier
#ifdef _DEBUG
                          , cabinetReplacer
#else
                          //, cabinetReplacer
                          , cabinet
#endif
                          , flanger
                          , reverberator
                         );
        y = split(y, delay1, delay2, 0.5f) * 1.5f;

        *outL = y;
        *outR = y;
    }

    void proccessChunk(float * input, float * outL, float * outR, long samples) {
        while(samples--)
            proccess(*input++, outL++, outR++);
    }
};

//AudioProcessor processor_;
//Playback playback;



