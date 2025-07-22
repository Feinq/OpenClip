//go:build windows

package capture

import (
	"fmt"
	"path/filepath"

	"github.com/Feinq/openclip/internal/config"
)

// FFmpeg arguments for capturing the screen on Windows.
func getPlatformFFmpegArgs(cfg *config.Config) []string {
	segmentTime := 2
	segmentCount := cfg.BufferTime / segmentTime

	args := []string{
		"-f", "gdigrab", // Windows-specific device
		"-i", "desktop", // Capture the entire desktop
		"-c:v", "libx264",
		"-preset", "ultrafast",
		"-f", "segment",
		"-segment_time", fmt.Sprintf("%d", segmentTime),
		"-segment_wrap", fmt.Sprintf("%d", segmentCount),
		"-reset_timestamps", "1",
		filepath.Join(cfg.BufferDir, "segment_%03d.ts"),
	}

	return args
}
