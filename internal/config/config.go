package config

import (
	"fmt"
	"os"
	"path/filepath"

	"gopkg.in/yaml.v3"
)

type Config struct {
	FFmpegPath string `yaml:"ffmpeg_path"`
	OutputDir  string `yaml:"output_dir"`
	BufferDir  string `yaml:"buffer_dir"`
	BufferTime int    `yaml:"buffer_time"` // in seconds
	Hotkey     string `yaml:"hotkey"`
	LogLevel   string `yaml:"log_level"`
}

func ConfigPath() (string, error) {
	configDir, err := os.UserConfigDir()
	if err != nil {
		return "", err
	}

	return filepath.Join(configDir, "openclip", "config.yaml"), nil
}

func DefaultConfig() *Config {
	return &Config{
		FFmpegPath: "ffmpeg",
		OutputDir:  "./output",
		BufferDir:  "./buffer",
		BufferTime: 30,
		Hotkey:     "F10",
		LogLevel:   "info",
	}
}

func LoadOrCreate() (*Config, error) {
	path, err := ConfigPath()
	if err != nil {
		return nil, err
	}

	if _, err := os.Stat(path); os.IsNotExist(err) {
		fmt.Println("No config file found, creating default config at:", path)
		cfg := DefaultConfig()

		if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
			return nil, fmt.Errorf("failed to create config directory: %w", err)
		}

		data, err := yaml.Marshal(cfg)
		if err != nil {
			return nil, fmt.Errorf("failed to marshal default config: %w", err)
		}

		if err := os.WriteFile(path, data, 0644); err != nil {
			return nil, fmt.Errorf("failed to write default config file: %w", err)
		}

		return cfg, nil
	}

	cfg := DefaultConfig()

	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read existing config file: %w", err)
	}

	if err := yaml.Unmarshal(data, cfg); err != nil {
		return nil, fmt.Errorf("failed to unmarshal config: %w", err)
	}

	updatedData, err := yaml.Marshal(cfg)
	if err != nil {
		return nil, fmt.Errorf("failed to marshal updated config: %w", err)
	}
	if err := os.WriteFile(path, updatedData, 0644); err != nil {
		return nil, fmt.Errorf("failed to write updated config file: %w", err)
	}

	fmt.Println("Config loaded from:", path)
	return cfg, nil
}
