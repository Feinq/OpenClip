#!/bin/bash
set -e

buildDir="./build"
goSourcePath="./cmd/openclip"
dllSourcePath="./native/OpenClipNative/x64/Release/OpenClipNative.dll"
ffmpegBinariesPath="./bin"

# Clean up the previous build directory
if [ -d "$buildDir" ]; then
    echo "Cleaning old build directory..."
    rm -rf "$buildDir"
fi
mkdir -p "$buildDir"

# Build the Go application
echo "Building Go application..."
go build -o "$buildDir/openclip.exe" "$goSourcePath"
if [ $? -ne 0 ]; then
    echo "Go build failed! Aborting." >&2
    exit 1
fi

# Copy required Native DLL
echo "Copying native DLLs..."
cp -f "$dllSourcePath" "$buildDir/"

# Copy FFmpeg binaries
echo "Copying FFmpeg binaries..."
cp -rf "$ffmpegBinariesPath/"* "$buildDir/"

echo -e "\e[32mBuild complete! Application is ready in the '$buildDir' directory.\e[0m"