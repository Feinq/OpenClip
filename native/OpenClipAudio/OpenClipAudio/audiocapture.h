#pragma once

#ifdef AUDIOCAPTURE_EXPORTS
#define AUDIOCAPTURE_API __declspec(dllexport)
#else
#define AUDIOCAPTURE_API __declspec(dllimport)
#endif

extern "C" {
    // Starts the audio capture thread.
    // Returns 0 on success, or a negative number on failure.
    AUDIOCAPTURE_API int StartCapture();

    // Stops the audio capture thread and cleans up resources.
    AUDIOCAPTURE_API void StopCapture();

    // Reads audio data from the internal circular buffer into a buffer provided by the caller.
    // pBuffer: A pointer to the buffer where audio data will be copied.
    // bufferSize: The maximum number of bytes to read.
    // Returns: The actual number of bytes read into the buffer. Can be 0 if no new data is available.
    AUDIOCAPTURE_API int ReadAudioBuffer(unsigned char* pBuffer, int bufferSize);

    // Gets the sample rate of the captured audio (e.g., 48000).
    AUDIOCAPTURE_API int GetSampleRate();

    // Gets the number of channels (e.g., 2 for stereo).
    AUDIOCAPTURE_API int GetChannels();
}