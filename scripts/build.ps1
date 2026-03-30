# EDA Project - Build Script
# Usage: .\scripts\build.ps1 [Debug|Release]

param(
    [string]$BuildType = "Release"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path $PSScriptRoot -Parent
$BuildDir = Join-Path $ProjectRoot "build"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "EDA Project - Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType"
Write-Host "Project: $ProjectRoot"
Write-Host "Build Dir: $BuildDir"
Write-Host ""

# Check dependencies
Write-Host "Checking dependencies..." -ForegroundColor Yellow
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    Write-Host "Error: CMake not found. Please install CMake 3.16+" -ForegroundColor Red
    exit 1
}

$qmake = Get-Command qmake -ErrorAction SilentlyContinue
if (-not $qmake) {
    Write-Host "Warning: qmake not found. Qt may not be configured." -ForegroundColor Yellow
}

# Create build directory
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Configure
Write-Host "Configuring CMake..." -ForegroundColor Green
Set-Location $BuildDir
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=$BuildType ..
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build
Write-Host "Building..." -ForegroundColor Green
cmake --build . --config $BuildType -j8
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

# Test
Write-Host "Running tests..." -ForegroundColor Green
ctest -C $BuildType --output-on-failure
if ($LASTEXITCODE -ne 0) {
    Write-Host "Tests failed!" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Success!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Executable: $BuildDir\$BuildType\bin\EDA.exe"
Write-Host ""
