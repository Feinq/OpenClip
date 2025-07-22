package capture

import (
	"fmt"
	"io"
	"os"
	"os/exec"
	"path/filepath"
	"sort"
	"strings"
	"syscall"
	"time"

	"github.com/Feinq/openclip/internal/audiocapture"
	"github.com/Feinq/openclip/internal/config"
	"github.com/Feinq/openclip/internal/logger"
)

type Capture struct {
	cfg    *config.Config
	log    logger.LoggerInterface
	cmd    *exec.Cmd
	stopCh chan struct{}
}

func NewCapture(cfg *config.Config, log logger.LoggerInterface) *Capture {
	return &Capture{
		cfg:    cfg,
		log:    log,
		stopCh: make(chan struct{}),
	}
}

func (c *Capture) Stop() {
	c.log.Info("Stopping capture process...")
	close(c.stopCh)

	if c.cmd != nil && c.cmd.Process != nil {
		err := c.cmd.Process.Signal(syscall.Signal(syscall.CTRL_BREAK_EVENT))
		if err != nil {
			c.log.Warnf("Failed to send interrupt to FFmpeg, killing process: %v", err)
			c.cmd.Process.Kill()
		}
	}
}

func (c *Capture) Start() {
	c.log.Info("Starting native audio capture...")
	audioStream, err := audiocapture.Start()
	if err != nil {
		c.log.Errorf("Failed to start native audio capture: %v", err)
		return
	}
	defer audiocapture.Stop()

	c.log.Info("Starting FFmpeg for video capture and encoding...")
	if err := os.RemoveAll(c.cfg.BufferDir); err != nil {
		c.log.Errorf("Failed to clean buffer directory: %v", err)
	}
	if err := os.MkdirAll(c.cfg.BufferDir, 0755); err != nil {
		c.log.Errorf("Failed to create buffer directory: %v", err)
		return
	}

	args := getPlatformFFmpegArgs(c.cfg, audioStream)
	c.cmd = exec.Command(c.cfg.FFmpegPath, args...)

	stdin, err := c.cmd.StdinPipe()
	if err != nil {
		c.log.Errorf("Failed to get FFmpeg stdin pipe: %v", err)
		return
	}
	c.cmd.Stdout = os.Stdout
	c.cmd.Stderr = os.Stderr

	if err := c.cmd.Start(); err != nil {
		c.log.Errorf("Failed to start ffmpeg: %v", err)
		return
	}
	c.log.Info("FFmpeg process started. Piping audio data...")

	go c.pipeAudio(stdin, audioStream)

	doneCh := make(chan error, 1)
	go func() {
		doneCh <- c.cmd.Wait()
	}()

	select {
	case <-c.stopCh:
		c.log.Info("Capture process is being stopped gracefully.")
	case err := <-doneCh:
		c.log.Warnf("FFmpeg process exited unexpectedly: %v", err)
	}
}

func (c *Capture) pipeAudio(stdin io.WriteCloser, stream *audiocapture.AudioStream) {
	defer stdin.Close()

	const bufferSize = 32768 // 32 KB buffer size
	buffer := make([]byte, bufferSize)

	for {
		select {
		case <-c.stopCh:
			c.log.Info("Audio piping stopped.")
			return
		default:
		}

		n, err := stream.Read(buffer)
		if err != nil {
			c.log.Warnf("Error reading from audio buffer: %v", err)
			continue
		}

		if n > 0 {
			if _, err := stdin.Write(buffer[:n]); err != nil {
				c.log.Warnf("Error writing to FFmpeg stdin (pipe likely closed): %v", err)
				return
			}
		}
	}
}

func (c *Capture) SaveClip() {
	c.log.Info("SaveClip triggered! Saving the last", c.cfg.BufferTime, "seconds.")
	tempDir, err := os.MkdirTemp("", "openclip-save-*")
	if err != nil {
		c.log.Errorf("Failed to create temp directory for saving: %v", err)
		return
	}
	defer os.RemoveAll(tempDir)
	c.log.Debugf("Created temp save directory: %s", tempDir)

	segments, err := filepath.Glob(filepath.Join(c.cfg.BufferDir, "*.ts"))
	if err != nil || len(segments) == 0 {
		c.log.Errorf("Could not find any video segments to save: %v", err)
		return
	}

	sort.Strings(segments)

	if len(segments) > 1 {
		segments = segments[:len(segments)-1]
	}

	copiedSegments := []string{}
	for _, segmentPath := range segments {
		sourceFile, err := os.Open(segmentPath)
		if err != nil {
			c.log.Warnf("Could not open segment %s for copying, skipping: %v", segmentPath, err)
			continue
		}

		destPath := filepath.Join(tempDir, filepath.Base(segmentPath))
		destFile, err := os.Create(destPath)
		if err != nil {
			c.log.Warnf("Could not create temp segment copy %s, skipping: %v", destPath, err)
			sourceFile.Close()
			continue
		}

		_, err = io.Copy(destFile, sourceFile)

		sourceFile.Close()
		destFile.Close()

		if err != nil {
			c.log.Warnf("Failed to copy segment %s, skipping: %v", segmentPath, err)
			continue
		}
		copiedSegments = append(copiedSegments, destPath)
	}

	if len(copiedSegments) == 0 {
		c.log.Error("Failed to copy any segments for saving.")
		return
	}

	playlistPath := filepath.Join(tempDir, "playlist.txt")
	playlist, err := os.Create(playlistPath)
	if err != nil {
		c.log.Errorf("Failed to create playlist file: %v", err)
		return
	}
	for _, s := range copiedSegments {
		playlist.WriteString(fmt.Sprintf("file '%s'\n", strings.ReplaceAll(s, "\\", "/")))
	}
	playlist.Close()

	if err := os.MkdirAll(c.cfg.OutputDir, 0755); err != nil {
		c.log.Errorf("Failed to create output directory: %v", err)
		return
	}
	relativeOutputFile := filepath.Join(c.cfg.OutputDir, fmt.Sprintf("clip_%s.mp4", time.Now().Format("20060102_150405")))
	outputFile, err := filepath.Abs(relativeOutputFile)
	if err != nil {
		c.log.Errorf("Failed to get absolute path for output file: %v", err)
		return
	}

	concatCmd := exec.Command(c.cfg.FFmpegPath,
		"-f", "concat",
		"-safe", "0",
		"-i", playlistPath,
		"-c", "copy",
		outputFile,
	)

	concatCmd.Stdout = os.Stdout
	concatCmd.Stderr = os.Stderr
	c.log.Infof("Saving clip with command: %s %s", c.cfg.FFmpegPath, strings.Join(concatCmd.Args[1:], " "))

	if err := concatCmd.Run(); err != nil {
		c.log.Errorf("Failed to save clip: %v", err)
		return
	}

	c.log.Infof("Successfully saved clip to: %s", outputFile)
}
