#pragma once

class AudioBackend {
public:
    virtual ~AudioBackend() = default;

    virtual int StartAudioCapture() = 0;
    virtual void StopAudioCapture() = 0;
    virtual int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) = 0;
    virtual int GetAudioSampleRate() = 0;
    virtual int GetAudioChannels() = 0;
};