package hotkey

import (
	"fmt"
	"strings"

	"github.com/Feinq/openclip/internal/logger"
	"golang.design/x/hotkey"
)

type Listener struct {
	log      logger.LoggerInterface
	callback func()
	hotkey   *hotkey.Hotkey
}

// Windows-specific implementation of parsing hotkeys.
func parseHotkey(hotkeyStr string) ([]hotkey.Modifier, hotkey.Key, error) {
	keyMap := map[string]hotkey.Key{
		"f1":  hotkey.KeyF1,
		"f2":  hotkey.KeyF2,
		"f3":  hotkey.KeyF3,
		"f4":  hotkey.KeyF4,
		"f5":  hotkey.KeyF5,
		"f6":  hotkey.KeyF6,
		"f7":  hotkey.KeyF7,
		"f8":  hotkey.KeyF8,
		"f9":  hotkey.KeyF9,
		"f10": hotkey.KeyF10,
		"f11": hotkey.KeyF11,
		"f12": hotkey.KeyF12,
	}
	key, ok := keyMap[strings.ToLower(hotkeyStr)]
	if !ok {
		return nil, 0, fmt.Errorf("unsupported hotkey: %s", hotkeyStr)
	}
	return []hotkey.Modifier{}, key, nil
}

func NewListener(hotkeyStr string, callback func(), log logger.LoggerInterface) (*Listener, error) {
	mods, key, err := parseHotkey(hotkeyStr)
	if err != nil {
		return nil, fmt.Errorf("invalid hotkey string: %w", err)
	}

	hk := hotkey.New(mods, key)
	return &Listener{
		log:      log,
		callback: callback,
		hotkey:   hk,
	}, nil
}

func (l *Listener) Listen() {
	l.log.Infof("Registering hotkey: [%s]", l.hotkey)
	err := l.hotkey.Register()
	if err != nil {
		l.log.Errorf("Failed to register hotkey: %v", err)
		return
	}
	defer l.hotkey.Unregister()

	l.log.Info("Hotkey listener is active. Press the hotkey to save a clip.")
	keydownChannel := l.hotkey.Keydown()

	for {
		<-keydownChannel
		l.log.Info("Hotkey triggered!")
		go l.callback()
	}
}
