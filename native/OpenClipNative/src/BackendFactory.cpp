#include <OpenClipNative/platform/AudioBackend.h>
#include <memory>

class WASAPIAudio;
class PipeWireAudio;

std::unique_ptr<AudioBackend> CreateAudioBackend();

#if defined(_WIN32) || defined(_WIN64)
#include "platform/windows/WASAPIAudio.h"
std::unique_ptr<AudioBackend> CreateAudioBackend() {
    return std::make_unique<WASAPIAudio>();
}
#elif defined(__linux__)
#include <OpenClipNative/platform/linux/PipeWireAudio.h>
std::unique_ptr<AudioBackend> CreateAudioBackend() {
    return std::make_unique<PipeWireAudio>();
}
#endif