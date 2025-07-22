package logger

import (
	"os"
	"path/filepath"

	"go.uber.org/zap"
	"go.uber.org/zap/zapcore"
)

type ZapLogger struct {
	sugared *zap.SugaredLogger
}

func NewZapLogger(logFilePath string, levelStr string) (*ZapLogger, error) {

	if err := os.MkdirAll(filepath.Dir(logFilePath), 0755); err != nil {
		return nil, err
	}

	level := parseLevel(levelStr)

	encoderCfg := zap.NewProductionEncoderConfig()
	encoderCfg.TimeKey = "time"
	encoderCfg.EncodeTime = zapcore.ISO8601TimeEncoder

	consoleEncoder := zapcore.NewConsoleEncoder(encoderCfg)
	consoleDebugPriority := zap.LevelEnablerFunc(func(lvl zapcore.Level) bool {
		return lvl >= level
	})

	consoleWriter := zapcore.Lock(os.Stdout)
	consoleCore := zapcore.NewCore(consoleEncoder, consoleWriter, consoleDebugPriority)

	fileEncoder := zapcore.NewJSONEncoder(encoderCfg)
	fileWriter, err := os.OpenFile(logFilePath, os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0644)
	if err != nil {
		return nil, err
	}

	fileCore := zapcore.NewCore(fileEncoder, zapcore.AddSync(fileWriter), level)

	combinedCore := zapcore.NewTee(
		consoleCore,
		fileCore,
	)

	logger := zap.New(combinedCore, zap.AddCaller())
	sugaredLogger := logger.Sugar()

	return &ZapLogger{sugared: sugaredLogger}, nil
}

func (l *ZapLogger) Info(args ...interface{}) {
	l.sugared.Info(args...)
}

func (l *ZapLogger) Infof(template string, args ...interface{}) {
	l.sugared.Infof(template, args...)
}

func (l *ZapLogger) Error(args ...interface{}) {
	l.sugared.Error(args...)
}

func (l *ZapLogger) Errorf(template string, args ...interface{}) {
	l.sugared.Errorf(template, args...)
}

func (l *ZapLogger) Debug(args ...interface{}) {
	l.sugared.Debug(args...)
}

func (l *ZapLogger) Debugf(template string, args ...interface{}) {
	l.sugared.Debugf(template, args...)
}

func (l *ZapLogger) Warn(args ...interface{}) {
	l.sugared.Warn(args...)
}

func (l *ZapLogger) Warnf(template string, args ...interface{}) {
	l.sugared.Warnf(template, args...)
}

func (l *ZapLogger) Fatal(args ...interface{}) {
	l.sugared.Fatal(args...)
}

func (l *ZapLogger) Fatalf(template string, args ...interface{}) {
	l.sugared.Fatalf(template, args...)
}
