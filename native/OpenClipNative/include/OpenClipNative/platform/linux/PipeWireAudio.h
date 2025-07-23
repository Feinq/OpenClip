
      

#if defined(__linux__)
#pragma once

#include <OpenClipNative/platform/AudioBackend.h>
#include <OpenClipNative/common/CircularBuffer.h>

#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

class PipeWireAudio : public AudioBackend {
public:
    PipeWireAudio();
    ~PipeWireAudio() override;

    int StartAudioCapture() override;
    void StopAudioCapture() override;
    int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) override;
    int GetAudioSampleRate() override;
    int GetAudioChannels() override;

private:
    void CaptureThreadFunc();

    
};

#endif // defined(__linux__)