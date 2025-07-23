#if defined(_WIN32) || defined(_WIN64)

#include "pch.h"
#include <OpenClipNative/platform/windows/WASAPIAudio.h>
#include <mmdeviceapi.h>
#include <stdexcept>

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

WASAPIAudio::WASAPIAudio() :
    m_pAudioClient(nullptr),
    m_pCaptureClient(nullptr),
    m_pWaveFormat(nullptr),
    m_isAudioCapturing(false)
{
}

// Destructor: Ensure cleanup happens.
WASAPIAudio::~WASAPIAudio() {
    StopAudioCapture();
}

void WASAPIAudio::CaptureThreadFunc() {
    if (FAILED(CoInitialize(NULL))) return;

    HRESULT hr;
    BYTE* pData;
    UINT32 numFramesAvailable;
    DWORD flags;

    hr = m_pAudioClient->Start();
    if (FAILED(hr)) return;

    while (m_isAudioCapturing) {

        hr = m_pCaptureClient->GetNextPacketSize(&numFramesAvailable);
        if (FAILED(hr)) continue;

        if (numFramesAvailable == 0) continue;

        hr = m_pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
        if (FAILED(hr)) continue;

        if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
            int bytesToWrite = numFramesAvailable * m_pWaveFormat->nBlockAlign;
            m_audioCircularBuffer.Write(pData, bytesToWrite);
        }

        m_pCaptureClient->ReleaseBuffer(numFramesAvailable);
    }

    m_pAudioClient->Stop();
    CoUninitialize();
}

int WASAPIAudio::StartAudioCapture() {
    if (m_isAudioCapturing) return 0;

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

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_pAudioClient);
    pDevice->Release();
    if (FAILED(hr)) return -4;

    hr = m_pAudioClient->GetMixFormat(&m_pWaveFormat);
    if (FAILED(hr)) return -5;

    hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, m_pWaveFormat, NULL);
    if (FAILED(hr)) return -6;

    hr = m_pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&m_pCaptureClient);
    if (FAILED(hr)) return -7;

    m_audioCircularBuffer.Resize(AUDIO_BUFFER_SIZE);

    m_isAudioCapturing = true;
    m_audioCaptureThread = std::thread(&WASAPIAudio::CaptureThreadFunc, this);

    return 0;
}

void WASAPIAudio::StopAudioCapture() {
    if (!m_isAudioCapturing) return;

    m_isAudioCapturing = false;
    if (m_audioCaptureThread.joinable()) {
        m_audioCaptureThread.join();
    }

    if (m_pCaptureClient) { m_pCaptureClient->Release(); m_pCaptureClient = nullptr; }
    if (m_pAudioClient) { m_pAudioClient->Release(); m_pAudioClient = nullptr; }
    if (m_pWaveFormat) { CoTaskMemFree(m_pWaveFormat); m_pWaveFormat = nullptr; }

    CoUninitialize();
}

int WASAPIAudio::ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) {
    if (!pBuffer || bufferSize == 0 || !m_isAudioCapturing) {
        return 0;
    }

    return static_cast<int>(m_audioCircularBuffer.Read(pBuffer, bufferSize));
}

int WASAPIAudio::GetAudioSampleRate() {
    if (!m_pWaveFormat) return 0;
    return m_pWaveFormat->nSamplesPerSec;
}

int WASAPIAudio::GetAudioChannels() {
    if (!m_pWaveFormat) return 0;
    return m_pWaveFormat->nChannels;
}

#endif // defined(_WIN32) || defined(_WIN64)