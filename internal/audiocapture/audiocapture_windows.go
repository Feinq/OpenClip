//go:build windows

package audiocapture

/*
#cgo CFLAGS: -I${SRCDIR}/../../native/OpenClipNative/include
#cgo LDFLAGS: -L${SRCDIR}/../../native/OpenClipNative/build/bin/Release -lOpenClipNative

#include <OpenClipNative/OCNative.h>
*/
import "C"
import "unsafe"

func startCapture() int {
	return int(C.StartAudioCapture())
}

func stopCapture() {
	C.StopAudioCapture()
}

func getAudioSampleRate() int {
	return int(C.GetAudioSampleRate())
}

func getAudioChannels() int {
	return int(C.GetAudioChannels())
}

func readAudioBuffer(p []byte) (int, error) {
	bytesRead := C.ReadAudioBuffer(
		(*C.uchar)(unsafe.Pointer(&p[0])),
		C.int(len(p)),
	)
	return int(bytesRead), nil
}
