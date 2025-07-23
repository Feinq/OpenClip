$buildDir = ".\build"
$goSourcePath = "./cmd/openclip"
$dllSourcePath = ".\native\OpenClipNative\x64\Release\OpenClipNative.dll"
$ffmpegBinariesPath = ".\bin"

# Clean up the previous build directory
if (Test-Path $buildDir) {
    Write-Host "Cleaning old build directory..."
    Remove-Item -Path $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir

# Build the Go application
Write-Host "Building Go application..."
go build -o "$buildDir\openclip.exe" $goSourcePath
if ($LASTEXITCODE -ne 0) {
    Write-Host "Go build failed! Aborting." -ForegroundColor Red
    exit 1
}

# Copy required Native DLL
Write-Host "Copying native DLLs..."
Copy-Item -Path $dllSourcePath -Destination $buildDir -Force # -Force to overwrite if it already exists

# Copy FFmpeg binaries
Write-Host "Copying FFmpeg binaries..."
Copy-Item -Path "$ffmpegBinariesPath\*" -Destination $buildDir -Recurse -Force

Write-Host "Build complete! Application is ready in the '$buildDir' directory." -ForegroundColor Green