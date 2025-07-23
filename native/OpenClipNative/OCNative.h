#pragma once

#ifdef OPENCLIP_NATIVE_EXPORTS
#define OPENCLIP_NATIVE_API __declspec(dllexport)
#else
#define OPENCLIP_NATIVE_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

    // Audio Capture Functions
    OPENCLIP_NATIVE_API int StartAudioCapture();
    OPENCLIP_NATIVE_API void StopAudioCapture();
    OPENCLIP_NATIVE_API int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize);
    OPENCLIP_NATIVE_API int GetAudioSampleRate();
    OPENCLIP_NATIVE_API int GetAudioChannels();

    // TODO: Video Capture Functions
    //OPENCLIP_NATIVE_API int StartVideoCapture(HWND targetWindowHandle);
    //OPENCLIP_NATIVE_API void StopVideoCapture();
    //OPENCLIP_NATIVE_API int ReadVideoBuffer(unsigned char* pBuffer, int bufferSize);
    //OPENCLIP_NATIVE_API int GetVideoWidth();
    //OPENCLIP_NATIVE_API int GetVideoHeight();
    //OPENCLIP_NATIVE_API int GetVideoFPS();

#ifdef __cplusplus
}
#endif