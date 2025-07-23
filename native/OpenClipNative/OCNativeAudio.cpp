#include "pch.h"
#include "OCNative.h"
#include "CircularBuffer.h"
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
std::thread audioCaptureThread;
std::atomic<bool> isAudioCapturing = false;
WAVEFORMATEX* pWaveFormat = nullptr;

CircularBuffer<BYTE> audioCircularBuffer;
const size_t AUDIO_BUFFER_SIZE = 48000 * 4 * 2 * 5; // 5 seconds of stereo 32-bit float audio at 48kHz

void AudioCaptureThreadFunc() {
    if (FAILED(CoInitialize(NULL))) return;

    HRESULT hr;
    BYTE* pData;
    UINT32 numFramesAvailable;
    DWORD flags;

    hr = pAudioClient->Start();
    if (FAILED(hr)) return;

    while (isAudioCapturing) {

        hr = pCaptureClient->GetNextPacketSize(&numFramesAvailable);
        if (FAILED(hr)) continue;

        if (numFramesAvailable == 0) continue;

        hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
        if (FAILED(hr)) continue;

        if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
            int bytesToWrite = numFramesAvailable * pWaveFormat->nBlockAlign;
            audioCircularBuffer.Write(pData, bytesToWrite);
        }

        pCaptureClient->ReleaseBuffer(numFramesAvailable);
    }

    pAudioClient->Stop();
    CoUninitialize();
}

OPENCLIP_NATIVE_API int StartAudioCapture() {
    if (isAudioCapturing) return 0;

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

    audioCircularBuffer.Resize(AUDIO_BUFFER_SIZE);

    isAudioCapturing = true;
    audioCaptureThread = std::thread(AudioCaptureThreadFunc);

    return 0;
}

OPENCLIP_NATIVE_API void StopAudioCapture() {
    if (!isAudioCapturing) return;

    isAudioCapturing = false;
    if (audioCaptureThread.joinable()) {
        audioCaptureThread.join();
    }

    if (pCaptureClient) { pCaptureClient->Release(); pCaptureClient = nullptr; }
    if (pAudioClient) { pAudioClient->Release(); pAudioClient = nullptr; }
    if (pWaveFormat) { CoTaskMemFree(pWaveFormat); pWaveFormat = nullptr; }

    CoUninitialize();
}

OPENCLIP_NATIVE_API int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) {
    if (!pBuffer || bufferSize == 0 || !isAudioCapturing) {
        return 0;
    }

    return static_cast<int>(audioCircularBuffer.Read(pBuffer, bufferSize));
}

OPENCLIP_NATIVE_API int GetAudioSampleRate() {
    if (!pWaveFormat) return 0;
    return pWaveFormat->nSamplesPerSec;
}

OPENCLIP_NATIVE_API int GetAudioChannels() {
    if (!pWaveFormat) return 0;
    return pWaveFormat->nChannels;
}