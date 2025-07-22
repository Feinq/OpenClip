// go:build windows

package audiocapture

/*
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L${SRCDIR} -lOpenClipAudio

#include "audiocapture.h"
*/
import "C"

import (
	"fmt"
	"unsafe"
)

type AudioStream struct {
	SampleRate int
	Channels   int
}

func Start() (*AudioStream, error) {
	result := C.StartCapture()
	if result != 0 {
		return nil, fmt.Errorf("failed to start audio capture, C error code: %d", int(result))
	}

	sampleRate := int(C.GetSampleRate())
	channels := int(C.GetChannels())

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
	C.StopCapture()
}

func (s *AudioStream) Read(p []byte) (int, error) {
	if len(p) == 0 {
		return 0, nil
	}

	bytesRead := C.ReadAudioBuffer(
		(*C.uchar)(unsafe.Pointer(&p[0])),
		C.int(len(p)),
	)

	return int(bytesRead), nil
}
