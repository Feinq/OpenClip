//go:build linux

package capture

import (
	"fmt"
	"path/filepath"

	"github.com/Feinq/openclip/internal/audiocapture"
	"github.com/Feinq/openclip/internal/config"
)

func getPlatformFFmpegArgs(cfg *config.Config, audioStream *audiocapture.AudioStream) []string {
	segmentTime := 2
	segmentCount := cfg.BufferTime / 2
	audioFormat := "f32le"

	return []string{
		"-f", "x11grab", "-i", ":0.0",
		"-f", audioFormat, "-ar", fmt.Sprintf("%d", audioStream.SampleRate), "-ac", fmt.Sprintf("%d", audioStream.Channels), "-i", "-",
		"-c:v", "libx264",
		"-preset", "ultrafast",
		"-pix_fmt", "yuv420p",
		"-c:a", "aac",
		"-b:a", "192k",
		"-f", "segment",
		"-segment_time", fmt.Sprintf("%d", segmentTime),
		"-segment_wrap", fmt.Sprintf("%d", segmentCount),
		"-reset_timestamps", "1",
		"-map", "0:v",
		"-map", "1:a",
		filepath.Join(cfg.BufferDir, "segment_%03d.ts"),
	}
}
