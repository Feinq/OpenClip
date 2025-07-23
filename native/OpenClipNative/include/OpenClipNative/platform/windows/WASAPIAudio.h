      

#if defined(_WIN32) || defined(_WIN64)
#pragma once

#include <OpenClipNative/platform/AudioBackend.h>
#include <OpenClipNative/common/CircularBuffer.h>

#include <Windows.h>
#include <audioclient.h>
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

class WASAPIAudio : public AudioBackend {
public:
    WASAPIAudio();
    ~WASAPIAudio() override;

    int StartAudioCapture() override;
    void StopAudioCapture() override;
    int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) override;
    int GetAudioSampleRate() override;
    int GetAudioChannels() override;

private:
    void CaptureThreadFunc();

    IAudioClient* m_pAudioClient;
    IAudioCaptureClient* m_pCaptureClient;
    WAVEFORMATEX* m_pWaveFormat;

    std::thread m_audioCaptureThread;
    std::atomic<bool> m_isAudioCapturing;
    
    CircularBuffer<BYTE> m_audioCircularBuffer;
    
    static const size_t AUDIO_BUFFER_SIZE = 48000 * 4 * 2 * 5; // 5 seconds of stereo 32-bit float audio at 48kHz
};

#endif // defined(_WIN32) || defined(_WIN64)