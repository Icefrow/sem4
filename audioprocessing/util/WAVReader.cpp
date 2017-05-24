#include "pch.h"

#include <util/WAVReader.h>
#include <util/Buffer.h>
#include <util/AudioConverter.h>

#include <memory>
#include <fstream>

#include <Mmreg.h>
#include "AudioData.h"
#include "ResourceLoader.h"
#include <sstream>

using namespace std;

/* -------------------------------------------------------------------- */
// Forward declarations
HRESULT LoadWAVAudioInMemory(const uint8_t * wavData,
                             size_t wavDataSize,
                             const WAVEFORMATEX ** wfx,
                             const uint8_t ** startAudio,
                             uint32_t * audioBytes);

/* -------------------------------------------------------------------- */
void readWAV(const uint8_t * mem, size_t size, AudioData & outData)
{
    const WAVEFORMATEX * wfx = nullptr;
    const uint8_t * startAudio = nullptr;
    uint32_t audioBytes = 0;

    const HRESULT hr = LoadWAVAudioInMemory(mem, size, &wfx,
                                            &startAudio, &audioBytes);

    if(hr != S_OK)
        throw runtime_error("Can not read WAV-file");

    unsigned samples = audioBytes / (wfx->nChannels * wfx->wBitsPerSample / 8);
    unsigned rate = wfx->nSamplesPerSec;

    switch(wfx->wFormatTag) {
    case WAVE_FORMAT_PCM:
        switch(wfx->wBitsPerSample) {
        case 16:
            switch(wfx->nChannels) {
            case 1:
                outData.createMono(samples, rate);
                int16toFloat32((int16_t *)startAudio, outData.data[0], samples);
                return;

            case 2:
                outData.createStereo(samples, rate);
                int16toFloat32((int16_t *)startAudio, outData.data[0], outData.data[1], samples);
                return;
            }

            break;

        case 24:
            switch(wfx->nChannels) {
            case 1:
                outData.createMono(samples, rate);
                int24toFloat32(startAudio, outData.data[0], samples);
                return;

            case 2:
                outData.createStereo(samples, rate);
                int24toFloat32(startAudio, outData.data[0], outData.data[1], samples);
                return;
            }

            break;

        case 32:
            switch(wfx->nChannels) {
            case 1:
                outData.createMono(samples, rate);
                int32toFloat32((int32_t *)startAudio, outData.data[0], samples);
                return;

            case 2:
                outData.createStereo(samples, rate);
                int32toFloat32((int32_t *)startAudio, outData.data[0], outData.data[1], samples);
                return;
            }

            break;
        }

        break;

    case WAVE_FORMAT_IEEE_FLOAT:
        switch(wfx->nChannels) {
        case 1:
            outData.createMono(samples, rate);
            memcpy(outData.data[0], startAudio, samples * sizeof(float));
            return;

        case 2:
            outData.createStereo(samples, rate);
            float32StereoTofloat32Mono((float *)startAudio, outData.data[0], outData.data[1], samples);
            return;
        }

        break;
    }

    throw  runtime_error("Unsupported WAV format");
}

inline
void readWAV(const void * mem, size_t size, AudioData & outData)
{
    readWAV(static_cast<const byte *>(mem), size, outData);
}

void readWAV(const string & fileName, AudioData & outData)
{
    ifstream file(fileName);
    size_t size = (size_t)file.tellg();

    Buffer<char> buffer(size);
    file.read(buffer.get(), size);

    readWAV(buffer.get(), size, outData);
}

void readWAV(HMODULE dllOrExe, const char * resourceName, AudioData & outData)
{
    Blob resource = loadResourceData(resourceName, "WAVE", dllOrExe);

    if(!resource.data || !resource.size) {
        stringstream text;
        text << "Can not load WAV from resources, error code=" << GetLastError();
        throw runtime_error(text.str());
    }

    readWAV(resource.data, resource.size, outData);
}

/* -------------------------------------------------------------------- */
// From MS DirectX Audio toolkit
// https://github.com/Microsoft/DirectXTK12/blob/master/Audio/WAVFileReader.cpp

namespace {

const uint32_t FOURCC_RIFF_TAG = 'FFIR';
const uint32_t FOURCC_FORMAT_TAG = ' tmf';
const uint32_t FOURCC_DATA_TAG = 'atad';
const uint32_t FOURCC_WAVE_FILE_TAG = 'EVAW';
const uint32_t FOURCC_XWMA_FILE_TAG = 'AMWX';
const uint32_t FOURCC_DLS_SAMPLE = 'pmsw';
const uint32_t FOURCC_MIDI_SAMPLE = 'lpms';
const uint32_t FOURCC_XWMA_DPDS = 'sdpd';
const uint32_t FOURCC_XMA_SEEK = 'kees';

#pragma pack(push,1)
struct RIFFChunk {
    uint32_t tag;
    uint32_t size;
};

struct RIFFChunkHeader {
    uint32_t tag;
    uint32_t size;
    uint32_t riff;
};

#pragma pack(pop)

static_assert(sizeof(RIFFChunk) == 8, "structure size mismatch");
static_assert(sizeof(RIFFChunkHeader) == 12, "structure size mismatch");

}   // namespace


//--------------------------------------------------------------------------------------
static const RIFFChunk * FindChunk(_In_reads_bytes_(sizeBytes) const uint8_t * data,
                                   _In_ size_t sizeBytes, _In_ uint32_t tag)
{
    if(!data)
        return nullptr;

    const uint8_t * ptr = data;
    const uint8_t * end = data + sizeBytes;

    while(end > (ptr + sizeof(RIFFChunk))) {
        auto header = reinterpret_cast<const RIFFChunk *>(ptr);

        if(header->tag == tag)
            return header;

        ptrdiff_t offset = header->size + sizeof(RIFFChunk);
        ptr += offset;
    }

    return nullptr;
}


//--------------------------------------------------------------------------------------
static HRESULT WaveFindFormatAndData(_In_reads_bytes_(wavDataSize) const uint8_t * wavData,
                                     _In_ size_t wavDataSize,
                                     _Outptr_ const WAVEFORMATEX ** pwfx, _Outptr_ const uint8_t ** pdata,
                                     _Out_ uint32_t * dataSize,
                                     _Out_ bool & dpds, _Out_ bool & seek)
{
    if(!wavData || !pwfx)
        return E_POINTER;

    dpds = seek = false;

    if(wavDataSize < (sizeof(RIFFChunk) * 2 + sizeof(uint32_t) + sizeof(WAVEFORMAT)))
        return E_FAIL;

    const uint8_t * wavEnd = wavData + wavDataSize;

    // Locate RIFF 'WAVE'
    auto riffChunk = FindChunk(wavData, wavDataSize, FOURCC_RIFF_TAG);

    if(!riffChunk || riffChunk->size < 4)
        return E_FAIL;

    auto riffHeader = reinterpret_cast<const RIFFChunkHeader *>(riffChunk);

    if(riffHeader->riff != FOURCC_WAVE_FILE_TAG && riffHeader->riff != FOURCC_XWMA_FILE_TAG)
        return E_FAIL;

    // Locate 'fmt '
    auto ptr = reinterpret_cast<const uint8_t *>(riffHeader) + sizeof(RIFFChunkHeader);

    if((ptr + sizeof(RIFFChunk)) > wavEnd)
        return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

    auto fmtChunk = FindChunk(ptr, riffHeader->size, FOURCC_FORMAT_TAG);

    if(!fmtChunk || fmtChunk->size < sizeof(PCMWAVEFORMAT))
        return E_FAIL;

    ptr = reinterpret_cast<const uint8_t *>(fmtChunk) + sizeof(RIFFChunk);

    if(ptr + fmtChunk->size > wavEnd)
        return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

    auto wf = reinterpret_cast<const WAVEFORMAT *>(ptr);

    // Validate WAVEFORMAT (focused on chunk size and format tag, not other data that XAUDIO2 will validate)
    switch(wf->wFormatTag) {
    case WAVE_FORMAT_PCM:
    case WAVE_FORMAT_IEEE_FLOAT:
        // Can be a PCMWAVEFORMAT (8 bytes) or WAVEFORMATEX (10 bytes)
        // We validiated chunk as at least sizeof(PCMWAVEFORMAT) above
        break;

    default: {
        if(fmtChunk->size < sizeof(WAVEFORMATEX))
            return E_FAIL;

        auto wfx = reinterpret_cast<const WAVEFORMATEX *>(ptr);

        if(fmtChunk->size < (sizeof(WAVEFORMATEX) + wfx->cbSize))
            return E_FAIL;

        switch(wfx->wFormatTag) {
        case WAVE_FORMAT_WMAUDIO2:
        case WAVE_FORMAT_WMAUDIO3:
            dpds = true;
            break;

        case  0x166 /*WAVE_FORMAT_XMA2*/: // XMA2 is supported by Xbox One
            if((fmtChunk->size < 52 /*sizeof(XMA2WAVEFORMATEX)*/)
               || (wfx->cbSize < 34 /*( sizeof(XMA2WAVEFORMATEX) - sizeof(WAVEFORMATEX) )*/))
                return E_FAIL;

            seek = true;
            break;

        case WAVE_FORMAT_ADPCM:
            if((fmtChunk->size < (sizeof(WAVEFORMATEX) + 32))
               || (wfx->cbSize < 32 /*MSADPCM_FORMAT_EXTRA_BYTES*/))
                return E_FAIL;

            break;

        case WAVE_FORMAT_EXTENSIBLE:
            if((fmtChunk->size < sizeof(WAVEFORMATEXTENSIBLE))
               || (wfx->cbSize < (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))))
                return E_FAIL;
            else {
                static const GUID s_wfexBase = { 0x00000000, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };

                auto wfex = reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(ptr);

                if(memcmp(reinterpret_cast<const BYTE *>(&wfex->SubFormat) + sizeof(DWORD),
                          reinterpret_cast<const BYTE *>(&s_wfexBase) + sizeof(DWORD),
                          sizeof(GUID) - sizeof(DWORD)) != 0)
                    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

                switch(wfex->SubFormat.Data1) {
                case WAVE_FORMAT_PCM:
                case WAVE_FORMAT_IEEE_FLOAT:
                    break;

                // MS-ADPCM and XMA2 are not supported as WAVEFORMATEXTENSIBLE

                case WAVE_FORMAT_WMAUDIO2:
                case WAVE_FORMAT_WMAUDIO3:
                    dpds = true;
                    break;

                default:
                    return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
                }
            }

            break;

        default:
            return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
        }
    }
    }

    // Locate 'data'
    ptr = reinterpret_cast<const uint8_t *>(riffHeader) + sizeof(RIFFChunkHeader);

    if((ptr + sizeof(RIFFChunk)) > wavEnd)
        return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

    auto dataChunk = FindChunk(ptr, riffChunk->size, FOURCC_DATA_TAG);

    if(!dataChunk || !dataChunk->size)
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

    ptr = reinterpret_cast<const uint8_t *>(dataChunk) + sizeof(RIFFChunk);

    if(ptr + dataChunk->size > wavEnd)
        return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

    *pwfx = reinterpret_cast<const WAVEFORMATEX *>(wf);
    *pdata = ptr;
    *dataSize = dataChunk->size;
    return S_OK;
}


//--------------------------------------------------------------------------------------
HRESULT LoadWAVAudioInMemory(const uint8_t * wavData,
                             size_t wavDataSize,
                             const WAVEFORMATEX ** wfx,
                             const uint8_t ** startAudio,
                             uint32_t * audioBytes)
{
    if(!wavData || !wfx || !startAudio || !audioBytes)
        return E_INVALIDARG;

    *wfx = nullptr;
    *startAudio = nullptr;
    *audioBytes = 0;

    // Need at least enough data to have a valid minimal WAV file
    if(wavDataSize < (sizeof(RIFFChunk) * 2 + sizeof(DWORD) + sizeof(WAVEFORMAT)))
        return E_FAIL;

    bool dpds, seek;
    HRESULT hr = WaveFindFormatAndData(wavData, wavDataSize, wfx, startAudio, audioBytes, dpds,
                                       seek);

    if(FAILED(hr))
        return hr;

    return (dpds || seek) ? E_FAIL : S_OK;
}
