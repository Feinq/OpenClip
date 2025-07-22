package logger

import (
	"fmt"
	"os"
	"path"
	"runtime"
	"time"
)

type BootstrapLogger struct{}

func NewBootstrapLogger() *BootstrapLogger {
	return &BootstrapLogger{}
}

func (b *BootstrapLogger) log(level string, msg string) {
	timestamp := time.Now().Format("2006-01-02T15:04:05.000-0700")

	_, file, line, ok := runtime.Caller(3)
	caller := "unknown"
	if ok {
		caller = fmt.Sprintf("%s:%d", path.Base(file), line)
	}

	fmt.Printf("%s\t%s\t%s\t%s\n", timestamp, level, caller, msg)
}

func (b *BootstrapLogger) Info(args ...interface{}) {
	b.log("info", fmt.Sprint(args...))
}

func (b *BootstrapLogger) Infof(format string, args ...interface{}) {
	b.log("info", fmt.Sprintf(format, args...))
}

func (b *BootstrapLogger) Warn(args ...interface{}) {
	b.log("warn", fmt.Sprint(args...))
}

func (b *BootstrapLogger) Warnf(format string, args ...interface{}) {
	b.log("warn", fmt.Sprintf(format, args...))
}

func (b *BootstrapLogger) Error(args ...interface{}) {
	b.log("error", fmt.Sprint(args...))
}

func (b *BootstrapLogger) Errorf(format string, args ...interface{}) {
	b.log("error", fmt.Sprintf(format, args...))
}

func (b *BootstrapLogger) Debug(args ...interface{}) {
	b.log("debug", fmt.Sprint(args...))
}

func (b *BootstrapLogger) Debugf(format string, args ...interface{}) {
	b.log("debug", fmt.Sprintf(format, args...))
}

func (b *BootstrapLogger) Fatal(args ...interface{}) {
	b.log("fatal", fmt.Sprint(args...))
	os.Exit(1)
}

func (b *BootstrapLogger) Fatalf(format string, args ...interface{}) {
	b.log("fatal", fmt.Sprintf(format, args...))
	os.Exit(1)
}
