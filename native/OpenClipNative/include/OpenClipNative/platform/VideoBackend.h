#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

class VideoBackend {
public:
    virtual ~VideoBackend() = default;

    // Starts the video capture for a specific target.
    // targetHandle: A window handle (HWND) to capture.
    //               A value of 0 or NULL typically means capture the primary display.
    virtual int StartVideoCapture(HWND targetHandle) = 0;

    virtual void StopVideoCapture() = 0;

    virtual int ReadVideoBuffer(unsigned char* pBuffer, int bufferSize) = 0;

    virtual int GetVideoWidth() = 0;
    virtual int GetVideoHeight() = 0;
};