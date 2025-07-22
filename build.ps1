$buildDir = ".\build"
$goSourcePath = "./cmd/openclip"
$dllSourcePath = ".\native\OpenClipAudio\x64\Release\OpenClipAudio.dll"
$internalAudioCapturePath = "$buildDir\internal\audiocapture"
$ffmpegSourcePath = ".\bin"

# Clean up the previous build directory
if (Test-Path $buildDir) {
    Write-Host "Cleaning old build directory..."
    Remove-Item $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir

# Copy OpenClipAudio.dll to internal/audiocapture/
if (-not (Test-Path $internalAudioCapturePath)) {
    New-Item -ItemType Directory -Path $internalAudioCapturePath -Force
}
Copy-Item -Path $dllSourcePath -Destination $internalAudioCapturePath

# Build the Go application
Write-Host "Building Go application..."
go build -o "$buildDir\openclip.exe" $goSourcePath
if ($LASTEXITCODE -ne 0) {
    Write-Host "Go build failed! Aborting." -ForegroundColor Red
    exit 1
}

# Copy required DLLs
Write-Host "Copying native DLLs..."
Copy-Item -Path $dllSourcePath -Destination $buildDir

# Copy FFmpeg binaries
Write-Host "Copying FFmpeg..."
Copy-Item -Path "$ffmpegSourcePath\*" -Destination $buildDir

Write-Host "Build complete! Application is ready in the '$buildDir' directory." -ForegroundColor Green