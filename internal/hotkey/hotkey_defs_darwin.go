//go:build darwin

package hotkey

import (
	"fmt"
	"strings"

	"golang.design/x/hotkey"
)

// Darwin-specific implementation of parsing hotkeys. (macOS)
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
