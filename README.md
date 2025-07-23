# OpenClip 🎬

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
![Go Version](https://img.shields.io/github/go-mod/go-version/Feinq/OpenClip)

**OpenClip** is a lightweight, hybrid Go/C++ screen recording tool, inspired by apps like Medal.tv and ShadowPlay, but fully open-source and built to be transparent and extensible.

It's being built as a cross-platform desktop tool with native hotkey support, circular buffer recording via FFmpeg, and eventually, native game-aware and audio-aware recording.

This is a work-in-progress, not production-ready project, but already functional and designed with clarity and simplicity in mind.

**Linux and macOS is currently not supported**, but is planned for future releases.

## 🧱 Project Status

Right now it's in early development. Here's what's working and what's coming next:

### v0.1
- [x] Config file auto-creation + YAML parsing
- [x] Custom logger (stdout + file with log level)
- [x] FFmpeg-based circular buffer with segment recording
- [x] Hotkey detection on Windows
- [x] Clip saving via hotkey

### v0.2 _(Upcoming)_
- [x] Initial audio capture (WASAPI on Windows)
- [ ] Game/process detection and smart folder naming
- [ ] Save clips into game-specific subfolders (e.g., `./output/Strinova/clip-1234.mp4`)
- [ ] Window-specific FFmpeg capture (targeting active game window)

### v0.3
- [ ] Native Windows video capture (DirectX hooking)
- [ ] Per-process audio capture via WASAPI
- [ ] Replace FFmpeg with native DLLs for performance
- [ ] Basic GUI for configuration and browsing
- [ ] Support for other operating systems (Linux (X11/Wayland), macOS)

## 🗺️ Roadmap

We're building this in small, stable chunks. Each release aims to be minimal but complete.

| Version | Goal                             | Status          |
|---------|----------------------------------|-----------------|
| v0.1    | FFmpeg-based CLI clipper         | ✅ Done         |
| v0.2    | Game-aware folders + audio proto | 🚧 In progress  |
| v0.3    | Native capture PoC (Windows)     | ⏳ Planned      |
| v1.0    | Full native pipeline + GUI       | 🔮 One day maybe|

## 🛠️ Building From Source

OpenClip is a hybrid Go/C++ project. To build it from source, you will need both the Go toolchain and a C++ compiler for the native audio capture module.

### Prerequisites

*   **Go**: Version 1.24 or later.
*   **CMake**: Version 3.20 or later.
*   **C++ Compiler**: A C++ compiler that supports C++17 or later
    -  **Windows**: Visual Studio 2022 (with "Desktop development with C++" workload) is recommended.
    -  **Linux**: GCC or Clang.
*   **FFmpeg**: (For the current capture backend) The full build from [Gyan.dev](https://www.gyan.dev/ffmpeg/builds/) is recommended.

### Build Steps

1.  **Clone the repository:**
    ```sh
    git clone https://github.com/Feinq/OpenClip.git
    cd OpenClip
    ```

2.  **Compile the Native DLL:**

-   **Windows:**
    Open a **Developer Command Prompt for Visual Studio** and run:
    ```sh
    cd native/OpenClipNative
    mkdir build
    cd build
    cmake .. -G "Visual Studio 17 2022" -A x64
    cmake --build . --config Release
    ```
-   **Linux:**
    ```sh
    cd native/OpenClipNative
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    ```

3.  **Build the Go Application:**

-   **Windows only:**
    You can now run the included PowerShell build script, which will copy all the necessary files into a clean `build` directory.
    ```powershell
    .\build.ps1
    ```

-  **Linux:**

    Or you can build manually, but you will have to make sure you have a copy of the `OpenClipNative.dll` in the directory the executable is present in.

    ```sh
    go build -o openclip.exe .\cmd\openclip\
    ```

4.  **Run the Application:**
    ```sh
    cd build
    .\openclip.exe
    ```

## 📄 License

OpenClip is licensed under the [MIT License](LICENSE).
Feel free to use, modify, and distribute it with attribution.

## 🤝 Contributing

Contributions are very welcome! Whether it's bug reports, feature requests, or code improvements, please open an issue or submit a pull request.
Make sure to follow the existing code style and include tests where appropriate.