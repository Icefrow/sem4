#include "pch.h"

#include <comdef.h>

#include "AsioHelper.h"
#include "GuidHelper.h"

#include <asio/iasiodrv.h>
#include <ui/Oscilloscop.h>

#include <algorithm>
#include "AudioConverter.h"
#include <set>

using namespace std;

//Oscilloscop * s_inputOsc = NULL;
//Oscilloscop * s_outputOsc = NULL;

extern void proccessAudio(float * input, float * outL, float * outR, long samples);

namespace {
// Forward declarations
const char * asioErrorMessage(ASIOError err);

// text description of stream type
const char * streamDescription(long streamType);

// returns length in bytes for stream of given type of given samples number
long streamSize(long streamType, long numSamples);

// stub, not used now
ASIOTime * asioBufferSwitchTimeInfo(ASIOTime * params, long doubleBufferIndex,
                                    ASIOBool directProcess);

/* -------------------------------------------------- */
class AsioException : public runtime_error
{
public:
    explicit AsioException(const string & message, ASIOError err)
        : runtime_error(message + " due to following reason: " + asioErrorMessage(err)) {}
};

/* -------------------------------------------------- */
void checkResult(ASIOError err, const char * text)
{
    if(err != ASE_OK)
        throw AsioException(text, err);
}

} // namespace

/* -------------------------------------------------- */
namespace Asio {

/* -------------------------------------------------- */
Driver::Driver(const CLSID & clsid, HWND hwnd)
    : asio_(NULL)
    , clsid_(clsid)
    , version_(0)
{
    CoInitialize(0);

    /* ------------------------------------------------------------ */
    // create COM-object
    cout << endl << "Creating ASIO device driver " << clsid << "...";
    HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER, clsid, (void **)&asio_);

    if(hr != S_OK) {
        _com_error errorToString(hr);

        cout << " FAILED:" << endl << errorToString.ErrorMessage() << endl;
        throw runtime_error(errorToString.ErrorMessage());
    }

    currentDriver_ = this;

    cout << " OK" << endl;

    char textBuffer[1024];

    /* ------------------------------------------------------------ */
    // read driver name and version
    asio_->getDriverName(textBuffer);
    name_ = textBuffer;
    version_ = asio_->getDriverVersion();

    cout << "Driver: " << name_ << ", ver " << version_ << endl;

    try {
        /* ------------------------------------------------------------ */
        // try to initialize driver
        cout << "Initializing... ";

        if(!asio_->init(hwnd)) {
            asio_->getErrorMessage(textBuffer);
            throw runtime_error(string("Can not init driver: ") + textBuffer);
        }

        cout << "OK" << endl;

        // read current sample rate
        checkResult(asio_->getSampleRate(&sampleRate_),
                    "Can not get current sample rate");

        cout << "Current sample rate: " << sampleRate_ << endl;

        //ñreateBuffers(-1, 0, 0);
    }
    catch(const exception & ex) {
        cout << "Error!!! " << ex.what() << endl;
    }
}

Driver::~Driver()
{
    if(started_)
        stop();

    if(asio_) {
        asio_->Release();
        cout << "Driver released ..." << endl;
    }

    assert(currentDriver_ == this || currentDriver_ == NULL);
    currentDriver_ = NULL;

    CoUninitialize();
}

void Driver::start()
{
    cout << "Starting ASIO... ";

    try {
        checkResult(asio_->start(), "Can not start driver");
        started_ = true;
        cout << "OK" << endl;
    }
    catch(const exception & ex) {
        cout << "FAILED: " << ex.what() << endl;
        throw;
    }
}

void Driver::stop()
{
    if(started_) {
        started_ = false;

        cout << "Stopping driver " << clsid() << "..." << endl;
        asio_->stop();
        cout << "Disposing buffers ..." << endl;
        asio_->disposeBuffers();
    }
}

void Driver::readInfo(DriverInfo & result) const
{
    /* ------------------------------------------------------------ */
    // read channel information
    long numInputChannels = 0, numOutputChannels = 0;
    checkResult(asio_->getChannels(&numInputChannels, &numOutputChannels),
                "Can not get channels");

    if(numInputChannels) {
        cout << "Input channels (" << numInputChannels << "):" << endl;

        /* ------------------------------------------------------------ */
        // read input channels info
        for(long i = 0; i < numInputChannels; i++) {
            ASIOChannelInfo tmp = { i,  ASIOTrue };

            checkResult(asio_->getChannelInfo(&tmp),
                        "Can not read channel information");

            cout << "  * " << tmp.name << ": " << streamDescription(tmp.type) << endl;
            result.inputChannels.push_back({ !!tmp.isActive, tmp.channelGroup, tmp.type, tmp.name });
        }
    }

    if(numOutputChannels) {
        cout << "Output channels (" << numOutputChannels << "):" << endl;

        /* ------------------------------------------------------------ */
        // read output channels info
        for(long i = 0; i < numOutputChannels; i++) {
            ASIOChannelInfo tmp = { i,  ASIOFalse };

            checkResult(asio_->getChannelInfo(&tmp),
                        "Can not read channel information");

            cout << "  * " << tmp.name << ": " << streamDescription(tmp.type) << endl;
            result.outputChannels.push_back({ !!tmp.isActive, tmp.channelGroup, tmp.type, tmp.name });
        }
    }

    /* ------------------------------------------------------------ */
    // get buffer information
    checkResult(asio_->getBufferSize(&result.bufferInfo.minSize,
                                     &result.bufferInfo.maxSize,
                                     &result.bufferInfo.preferredSize,
                                     &result.bufferInfo.granularity),
                "Can not read buffer information");

    cout << "Buffer information: " << endl
         << "  * minSize: " << result.bufferInfo.minSize << endl
         << "  * maxSize: " << result.bufferInfo.maxSize << endl
         << "  * preferredSize: " << result.bufferInfo.preferredSize << endl
         << "  * granularity: " << result.bufferInfo.granularity << endl;

    /* ------------------------------------------------------------ */
    // read supported sample rate
    cout << "Supported sample rates:" << endl;

    {
        set<double> rates = { 24000, 44100, 48000, 96000 };
        rates.insert(sampleRate_);

        for(const double rate : rates) {
            if(asio_->canSampleRate(rate) == ASE_OK) {
                result.supportedSampleRates.push_back(rate);
                cout << "  * " << rate << endl;
            }
        }
    }
}

void Driver::showControlPanel()
{
    checkResult(asio_->controlPanel(),
                "Can not show ASIO control panel");
}

void Driver::setSampleRate(double sampleRate)
{
    if(sampleRate_ != sampleRate) {

        checkResult(asio_->setSampleRate(sampleRate),
                    "Can not set sample rate");

        sampleRate_ = sampleRate;
        cout << "Sample rate changed to: " << sampleRate << endl;
    }
}

namespace {
// global data
ASIOCallbacks s_callbacks;

struct SwitchingBuffer {
    void * input;
    void * outL;
    void * outR;
};

SwitchingBuffer s_buffers[2];   // 0/1 switching buffers
long s_bufferSamples;

}

void Driver::createBuffers(int inputChan, int outChanL, int outChanR, long bufferSize)
{
    asio_->disposeBuffers();

    cout << "Creating buffers:" << endl
         << "  * intput channel: " << inputChan << endl
         << "  * output channel L: " << outChanL << endl
         << "  * output channel R: " << outChanR << endl
         << "  * bufferSize: " << bufferSize << endl;

    s_callbacks = {
        bufferSwitch,
        asioSampleRateDidChangeCallback,
        asioMessageCallBack,
        asioBufferSwitchTimeInfo
    };

    if(inputChan == -1) {
        // do not create buffers, just provide callbacks
        int nbTotalChannels = 0;
        ASIOBufferInfo bi = { 0 };
        ASIOError err = asio_->createBuffers(NULL, 0, bufferSize, &s_callbacks);
        return;
    }

    //assert(inputChan >= 0 && inputChan < (int)inputChannels_.size());
    //assert(outChanL >= 0 && outChanL < (int)outputChannels.size());
    //assert(outChanR >= 0 && outChanR < (int)outputChannels.size());

    ASIOBufferInfo bufs[3]; // 0 - input, 1 - output/L, 2 - output/R
    memset(bufs, 0, sizeof(bufs));
    bufs[0].isInput = ASIOTrue;
    bufs[0].channelNum = inputChan;
    bufs[1].isInput = ASIOFalse;
    bufs[1].channelNum = outChanL;
    bufs[2].isInput = ASIOFalse;
    bufs[2].channelNum = outChanR;

    s_bufferSamples = bufferSize;

    ASIOError err = asio_->createBuffers(bufs, 3, s_bufferSamples, &s_callbacks);
    checkResult(err, "Can not create ASIO buffers");

    s_buffers[0] = { bufs[0].buffers[0], bufs[1].buffers[0], bufs[2].buffers[0] };
    s_buffers[1] = { bufs[0].buffers[1], bufs[1].buffers[1], bufs[2].buffers[1] };
}
/* -------------------------------------------------- */
Driver * Driver::currentDriver_ = NULL;

/* -------------------------------------------------- */
void Driver::bufferSwitch(long doubleBufferIndex, long directProcess)
{
    SwitchingBuffer buf = s_buffers[doubleBufferIndex];

    int32toFloat32inPlace((long *)buf.input, s_bufferSamples);

    //s_inputOsc->consume((float *)buf.input, s_bufferSamples);
    proccessAudio((float *)buf.input, (float *)buf.outL, (float *)buf.outR, s_bufferSamples);
    //s_outputOsc->consume((float *)buf.outL, s_bufferSamples);

    float32toInt32inPlace((float *)buf.outL, s_bufferSamples);
    float32toInt32inPlace((float *)buf.outR, s_bufferSamples);
}

/* -------------------------------------------------- */
long Driver::asioMessageCallBack(long selector, long value, void * message, double * opt)
{
    switch(selector) {
    case kAsioSelectorSupported:
        switch(value) {
        case kAsioEngineVersion:
            return 1;

        case kAsioResetRequest:
            return 1;

        case kAsioBufferSizeChange:
            return 0;

        case kAsioResyncRequest:
            return 0;

        case kAsioLatenciesChanged:
            return 0;

        case kAsioSupportsTimeInfo:
            return 0;

        case kAsioSupportsTimeCode:
            return 0;
        }

        break;

    case kAsioEngineVersion:
        return 2;

    case kAsioResetRequest:
        if(currentDriver_ && currentDriver_->driverWantsResetCallback_)
            currentDriver_->driverWantsResetCallback_();

        return 0;

    case kAsioBufferSizeChange:
        return 0;

    case kAsioResyncRequest:
        return 0;

    case kAsioLatenciesChanged:
        return 0;

    case kAsioSupportsTimeInfo:
        return 0;

    case kAsioSupportsTimeCode:
        return 0;

    case kAsioOverload:
        cout << "X";
        return 0;
    }

    return 0;
}

void Driver::asioSampleRateDidChangeCallback(double sRate)
{
    // TODO: do we need react on this failure during playing?
}

}   // namespace Asio

namespace {

ASIOTime * asioBufferSwitchTimeInfo(ASIOTime * params, long doubleBufferIndex,
                                    ASIOBool directProcess)
{
    return NULL;
}



/* -------------------------------------------------- */
const char * asioErrorMessage(ASIOError err)
{
    switch(err) {
    case ASE_OK:
        return "This value will be returned whenever the call succeeded";

    case ASE_SUCCESS:
        return "unique success return value for ASIOFuture calls";

    case ASE_NotPresent:
        return "hardware input or output is not present or available";

    case ASE_HWMalfunction:
        return "hardware is malfunctioning (can be returned by any ASIO function)";

    case ASE_InvalidParameter:
        return "input parameter invalid";

    case ASE_InvalidMode:
        return "hardware is in a bad mode or used in a bad mode";

    case ASE_SPNotAdvancing:
        return "hardware is not running when sample position is inquired";

    case ASE_NoClock:
        return "sample clock or rate cannot be determined or is not present";

    case ASE_NoMemory:
        return "not enough memory for completing the request";

    default:
        return "Unknown error";
    };
}

/* -------------------------------------------------- */
const char * streamDescription(long streamType)
{
    switch(streamType) {
    case ASIOSTInt16MSB:
        return "int 16 MSB";

    case ASIOSTInt24MSB:
        return "int 24 MSB";

    case ASIOSTInt32MSB:
        return "int 32 MSB";

    case ASIOSTFloat32MSB:
        return "float 32 MSB";

    case ASIOSTFloat64MSB:
        return "float 64 MSB";

    case ASIOSTInt32MSB16:
        return "int 32 MSB with 16 bit alignment";

    case ASIOSTInt32MSB18:
        return "int 32 MSB with 18 bit alignment";

    case ASIOSTInt32MSB20:
        return "int 32 MSB with 20 bit alignment";

    case ASIOSTInt32MSB24:
        return "int 32 MSB with 24 bit alignment";

    case ASIOSTInt16LSB:
        return "int 16 LSB";

    case ASIOSTInt24LSB:
        return "int 24 LSB";

    case ASIOSTInt32LSB:
        return "int 32 LSB";

    case ASIOSTFloat32LSB:
        return "float 32 LSB";

    case ASIOSTFloat64LSB:
        return "float 64 LSB";

    case ASIOSTInt32LSB16:
        return "int 32 LSB with 16 bit alignment";

    case ASIOSTInt32LSB18:
        return "int 32 LSB with 18 bit alignment";

    case ASIOSTInt32LSB20:
        return "int 32 LSB with 20 bit alignment";

    case ASIOSTInt32LSB24:
        return "int 32 LSB with 24 bit alignment";

    case ASIOSTDSDInt8LSB1:
        return "DSD 1 bit data, 8 samples per byte LSB";

    case ASIOSTDSDInt8MSB1:
        return "DSD 1 bit data, 8 samples per byte. MSB";

    case ASIOSTDSDInt8NER8:
        return "DSD 8 bit data, 1 sample per byte";

    default:
        return "invalid format";
    }
}

/* -------------------------------------------------- */
long streamSize(long streamType, long numSamples)
{
    switch(streamType) {
    case ASIOSTInt16MSB:
    case ASIOSTInt16LSB:
        return 2 * numSamples;

    case ASIOSTInt24MSB:
    case ASIOSTInt24LSB:
        return 3 * numSamples;

    case ASIOSTInt32MSB:
    case ASIOSTInt32LSB:
    case ASIOSTInt32MSB16:
    case ASIOSTInt32LSB16:
    case ASIOSTInt32MSB18:
    case ASIOSTInt32LSB18:
    case ASIOSTInt32MSB20:
    case ASIOSTInt32LSB20:
    case ASIOSTInt32MSB24:
    case ASIOSTInt32LSB24:
    case ASIOSTFloat32MSB:
    case ASIOSTFloat32LSB:
        return 4 * numSamples;

    case ASIOSTFloat64MSB:
    case ASIOSTFloat64LSB:
        return 8 * numSamples;

    case ASIOSTDSDInt8LSB1:
        return numSamples / 8;  // DSD 1 bit data, 8 samples per byte LSB

    case ASIOSTDSDInt8MSB1:
        return numSamples / 8;  // DSD 1 bit data, 8 samples per byte. MSB

    case ASIOSTDSDInt8NER8:
        return numSamples;

    default:
        throw domain_error("invalid format");
    }
}


} // namespace