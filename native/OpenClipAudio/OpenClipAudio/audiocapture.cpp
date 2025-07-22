#include "pch.h"
#include "audiocapture.h"
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <stdexcept>
#include <algorithm>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

IAudioClient* pAudioClient = nullptr;
IAudioCaptureClient* pCaptureClient = nullptr;
std::thread captureThread;
std::atomic<bool> isCapturing = false;
WAVEFORMATEX* pWaveFormat = nullptr;

std::vector<BYTE> circularBuffer;
std::mutex bufferMutex;
size_t bufferWritePos = 0;
size_t bufferReadPos = 0;
size_t bufferDataSize = 0;
const size_t BUFFER_SIZE = 48000 * 4 * 2 * 5; // 5 seconds of stereo 32-bit float audio at 48kHz

void WriteToCircularBuffer(const BYTE* data, size_t bytes) {
    std::lock_guard<std::mutex> lock(bufferMutex);

    size_t spaceAvailable = BUFFER_SIZE - bufferDataSize;
    if (bytes > spaceAvailable) {
        bufferReadPos = (bufferReadPos + (bytes - spaceAvailable)) % BUFFER_SIZE;
        bufferDataSize -= (bytes - spaceAvailable);
    }

    size_t firstChunk = std::min(bytes, BUFFER_SIZE - bufferWritePos);
    memcpy(&circularBuffer[bufferWritePos], data, firstChunk);

    if (bytes > firstChunk) {
        memcpy(&circularBuffer[0], data + firstChunk, bytes - firstChunk);
    }

    bufferWritePos = (bufferWritePos + bytes) % BUFFER_SIZE;
    bufferDataSize += bytes;
}

size_t ReadFromCircularBuffer(BYTE* dest, size_t bytes) {
    std::lock_guard<std::mutex> lock(bufferMutex);

    size_t bytesToRead = std::min(bytes, bufferDataSize);
    size_t firstChunk = std::min(bytesToRead, BUFFER_SIZE - bufferReadPos);

    memcpy(dest, &circularBuffer[bufferReadPos], firstChunk);
    if (bytesToRead > firstChunk) {
        memcpy(dest + firstChunk, &circularBuffer[0], bytesToRead - firstChunk);
    }

    bufferReadPos = (bufferReadPos + bytesToRead) % BUFFER_SIZE;
    bufferDataSize -= bytesToRead;

    return bytesToRead;
}

void CaptureThreadFunc() {
    if (FAILED(CoInitialize(NULL))) return;

    HRESULT hr;
    BYTE* pData;
    UINT32 numFramesAvailable;
    DWORD flags;

    hr = pAudioClient->Start();
    if (FAILED(hr)) return;

    while (isCapturing) {

        hr = pCaptureClient->GetNextPacketSize(&numFramesAvailable);
        if (FAILED(hr)) continue;

        if (numFramesAvailable == 0) continue;

        hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
        if (FAILED(hr)) continue;

        if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
            int bytesToWrite = numFramesAvailable * pWaveFormat->nBlockAlign;
            WriteToCircularBuffer(pData, bytesToWrite);
        }

        pCaptureClient->ReleaseBuffer(numFramesAvailable);
    }

    pAudioClient->Stop();
    CoUninitialize();
}

AUDIOCAPTURE_API int StartCapture() {
    if (isCapturing) return 0;

    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return -1;

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    if (FAILED(hr)) return -2;

    IMMDevice* pDevice = nullptr;
    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    pEnumerator->Release();
    if (FAILED(hr)) return -3;

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    pDevice->Release();
    if (FAILED(hr)) return -4;

    hr = pAudioClient->GetMixFormat(&pWaveFormat);
    if (FAILED(hr)) return -5;

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pWaveFormat, NULL);
    if (FAILED(hr)) return -6;

    hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
    if (FAILED(hr)) return -7;

    circularBuffer.resize(BUFFER_SIZE);
    bufferWritePos = 0;
    bufferReadPos = 0;
    bufferDataSize = 0;

    isCapturing = true;
    captureThread = std::thread(CaptureThreadFunc);

    return 0;
}

AUDIOCAPTURE_API void StopCapture() {
    if (!isCapturing) return;

    isCapturing = false;
    if (captureThread.joinable()) {
        captureThread.join();
    }

    if (pCaptureClient) { pCaptureClient->Release(); pCaptureClient = nullptr; }
    if (pAudioClient) { pAudioClient->Release(); pAudioClient = nullptr; }
    if (pWaveFormat) { CoTaskMemFree(pWaveFormat); pWaveFormat = nullptr; }

    CoUninitialize();
}

AUDIOCAPTURE_API int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) {
    if (!pBuffer || bufferSize == 0 || !isCapturing) {
        return 0;
    }

    return static_cast<int>(ReadFromCircularBuffer(pBuffer, bufferSize));
}

AUDIOCAPTURE_API int GetSampleRate() {
    if (!pWaveFormat) return 0;
    return pWaveFormat->nSamplesPerSec;
}

AUDIOCAPTURE_API int GetChannels() {
    if (!pWaveFormat) return 0;
    return pWaveFormat->nChannels;
}