package hotkey

import (
	"fmt"

	"github.com/Feinq/openclip/internal/logger"
	"golang.design/x/hotkey"
)

type Listener struct {
	log      logger.LoggerInterface
	callback func()
	hotkey   *hotkey.Hotkey
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
