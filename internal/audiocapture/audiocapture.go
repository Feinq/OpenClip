package audiocapture

import (
	"fmt"
)

type AudioStream struct {
	SampleRate int
	Channels   int
}

func Start() (*AudioStream, error) {
	result := startCapture()
	if result != 0 {
		return nil, fmt.Errorf("failed to start audio capture, C error code: %d", int(result))
	}

	sampleRate := getAudioSampleRate()
	channels := getAudioChannels()

	if sampleRate == 0 || channels == 0 {
		Stop()
		return nil, fmt.Errorf("failed to get audio format after starting capture")
	}

	stream := &AudioStream{
		SampleRate: sampleRate,
		Channels:   channels,
	}

	return stream, nil
}

func Stop() {
	stopCapture()
}

func (s *AudioStream) Read(p []byte) (int, error) {
	if len(p) == 0 {
		return 0, nil
	}
	return readAudioBuffer(p)
}
