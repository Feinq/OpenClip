#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
  #ifdef OPENCLIP_NATIVE_EXPORTS
    #define OPENCLIP_NATIVE_API __declspec(dllexport)
  #else
    #define OPENCLIP_NATIVE_API __declspec(dllimport)
  #endif
#else
  #define OPENCLIP_NATIVE_API __attribute__((visibility("default")))
#endif // defined(_WIN32) || defined(_WIN64)

#if !defined(_WIN32)
typedef void* HWND;
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  OPENCLIP_NATIVE_API int StartAudioCapture();
  OPENCLIP_NATIVE_API void StopAudioCapture();
  OPENCLIP_NATIVE_API int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize);
  OPENCLIP_NATIVE_API int GetAudioSampleRate();
  OPENCLIP_NATIVE_API int GetAudioChannels();

  OPENCLIP_NATIVE_API int StartVideoCapture(HWND targetWindowHandle);
  OPENCLIP_NATIVE_API void StopVideoCapture();
  OPENCLIP_NATIVE_API int ReadVideoBuffer(unsigned char* pBuffer, int bufferSize);
  OPENCLIP_NATIVE_API int GetVideoWidth();
  OPENCLIP_NATIVE_API int GetVideoHeight();

#ifdef __cplusplus
}
#endif // __cplusplus