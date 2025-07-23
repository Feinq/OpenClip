#include <OpenClipNative/OCNative.h>
#include <OpenClipNative/platform/AudioBackend.h>
#include <memory>

std::unique_ptr<AudioBackend> CreateAudioBackend();
static std::unique_ptr<AudioBackend> backend = CreateAudioBackend();

extern "C" {
    int StartAudioCapture() { return backend->StartAudioCapture(); }
    void StopAudioCapture() { backend->StopAudioCapture(); }
    int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize) { return backend->ReadAudioBuffer(pBuffer, bufferSize); }
    int GetAudioSampleRate() { return backend->GetAudioSampleRate(); }
    int GetAudioChannels() { return backend->GetAudioChannels(); }
}