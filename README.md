# OpenClip 🎬

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
![Go Version](https://img.shields.io/github/go-mod/go-version/Feinq/OpenClip)

**OpenClip** is a lightweight screen recording tool, inspired by apps like Medal.tv and ShadowPlay, but fully open-source and built to be transparent and extensible.

It's being built as a cross-platform desktop tool (Windows + Linux first) with native hotkey support, circular buffer recording via FFmpeg, and eventually, native game-aware and audio-aware recording.

This is a work-in-progress, not production-ready project, but already functional and designed with clarity and simplicity in mind.

## 🧱 Project Status

Right now it's in early development. Here's what's working and what's coming next:

### v0.1
- [x] Config file auto-creation + YAML parsing
- [x] Custom logger (stdout + file with log level)
- [x] FFmpeg-based circular buffer with segment recording
- [x] Hotkey detection on Windows + Linux (X11)
- [x] Clip saving via hotkey

### v0.2 _(Upcoming)_
- [ ] Initial audio capture (WASAPI on Windows)
- [ ] Game/process detection and smart folder naming
- [ ] Save clips into game-specific subfolders (e.g., `./output/Strinova/clip-1234.mp4`)
- [ ] Window-specific FFmpeg capture (targeting active game window)

### v0.3
- [ ] Native Windows video capture (DirectX hooking)
- [ ] Per-process audio capture via WASAPI
- [ ] Replace FFmpeg with native DLLs for performance
- [ ] Basic GUI for configuration and browsing

## 🗺️ Roadmap

We're building this in small, stable chunks. Each release aims to be minimal but complete.

| Version | Goal                             | Status          |
|---------|----------------------------------|-----------------|
| v0.1    | FFmpeg-based CLI clipper         | ✅ Done         |
| v0.2    | Game-aware folders + audio proto | 🚧 In progress  |
| v0.3    | Native capture PoC (Windows)     | ⏳ Planned      |
| v1.0    | Full native pipeline + GUI       | 🔮 One day maybe|

## 🛠️ Requirements

- **Go** 1.24+
- **FFmpeg** in PATH, used for video capture in current implementation (v0.1)

### Windows
- Use the essential or full [Gyan.dev FFmpeg build](https://www.gyan.dev/ffmpeg/builds/)

### Linux _(Tested on Arch Linux)_

- Requires **X11** (Wayland not yet supported)
- Install FFmpeg via your package manager (e.g., `sudo pacman -S ffmpeg`)

### macOS _(Not tested)_
- Download FFmpeg via the official site or use Homebrew: `brew install ffmpeg`

## 🚀 Running

```bash
git clone https://github.com/Feinq/OpenClip.git
cd OpenClip
go run ./cmd/openclip
```

## 📄 License

OpenClip is licensed under the [MIT License](LICENSE).
Feel free to use, modify, and distribute it with attribution.

## 🤝 Contributing

Contributions are very welcome! Whether it's bug reports, feature requests, or code improvements, please open an issue or submit a pull request.
Make sure to follow the existing code style and include tests where appropriate.