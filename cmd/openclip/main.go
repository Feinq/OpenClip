package main

import (
	"os"
	"os/signal"
	"syscall"

	"github.com/Feinq/openclip/internal/capture"
	"github.com/Feinq/openclip/internal/config"
	"github.com/Feinq/openclip/internal/hotkey"
	"github.com/Feinq/openclip/internal/logger"
)

var log logger.LoggerInterface

func main() {
	log = logger.NewBootstrapLogger()
	log.Info("Starting OpenClip...")

	cfg, err := config.LoadOrCreate()
	if err != nil {
		log.Error("Failed to load config:", err)
		os.Exit(1)
	}
	log.Info("Configuration loaded.")

	logFile := cfg.OutputDir + "/openclip.log"
	zapLogger, err := logger.NewZapLogger(logFile, cfg.LogLevel)
	if err != nil {
		log.Error("Failed to initialize full logger:", err)
		os.Exit(1)
	}

	log = zapLogger
	log.Infof("Log level from config: [%s]", cfg.LogLevel)

	log.Info("Initializing capture module...")
	capt := capture.NewCapture(cfg, log)

	log.Info("Initializing hotkey module...")
	hotkeyListener, err := hotkey.NewListener(cfg.Hotkey, capt.SaveClip, log)
	if err != nil {
		log.Fatalf("Failed to create hotkey listener: %v", err)
	}

	go capt.Start()

	go hotkeyListener.Listen()

	log.Info("OpenClip is running. Press Ctrl+C to exit.")
	shutdown := make(chan os.Signal, 1)
	signal.Notify(shutdown, syscall.SIGINT, syscall.SIGTERM)

	<-shutdown

	log.Info("Shutdown signal received, cleaning up...")

	capt.Stop()

	log.Info("OpenClip has been shut down.")
}
