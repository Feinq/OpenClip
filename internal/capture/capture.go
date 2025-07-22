package capture

import (
	"fmt"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"time"

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

func (c *Capture) Start() {
	c.log.Info("Starting capture...")

	if err := os.RemoveAll(c.cfg.BufferDir); err != nil {
		c.log.Errorf("Failed to clean buffer directory: %v", err)
	}
	if err := os.MkdirAll(c.cfg.BufferDir, 0755); err != nil {
		c.log.Errorf("Failed to create buffer directory: %v", err)
		return
	}

	args := getPlatformFFmpegArgs(c.cfg)
	c.cmd = exec.Command(c.cfg.FFmpegPath, args...)
	c.cmd.Stdout = os.Stdout
	c.cmd.Stderr = os.Stderr

	c.log.Infof("Executing FFmpeg command: %s %s", c.cfg.FFmpegPath, strings.Join(args, " "))
	if err := c.cmd.Start(); err != nil {
		c.log.Errorf("Failed to start ffmpeg: %v", err)
		return
	}

	c.log.Info("FFmpeg circular buffer capture has started.")

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

func (c *Capture) Stop() {
	c.log.Info("Stopping capture process...")
	close(c.stopCh)

	if c.cmd != nil && c.cmd.Process != nil {
		if err := c.cmd.Process.Signal(os.Interrupt); err != nil {
			c.log.Warnf("Failed to send interrupt to FFmpeg, killing process instead: %v", err)
			c.cmd.Process.Kill()
		}
	}
}

func (c *Capture) SaveClip() {
	c.log.Info("SaveClip triggered! Saving the last", c.cfg.BufferTime, "seconds.")

	segments, err := filepath.Glob(filepath.Join(c.cfg.BufferDir, "*.ts"))
	if err != nil || len(segments) == 0 {
		c.log.Errorf("Could not find any video segments to save: %v", err)
		return
	}

	playlistPath := filepath.Join(c.cfg.BufferDir, "playlist.txt")
	playlist, err := os.Create(playlistPath)
	if err != nil {
		c.log.Errorf("Failed to create playlist file: %v", err)
		return
	}

	for _, s := range segments {
		absPath, err := filepath.Abs(s)
		if err != nil {
			c.log.Errorf("Failed to get absolute path for segment %s: %v", s, err)
			playlist.Close()
			return
		}
		playlist.WriteString(fmt.Sprintf("file '%s'\n", absPath))
	}
	playlist.Close()
	defer os.Remove(playlistPath)

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
