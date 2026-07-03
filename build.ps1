# Caro Game Build Script for Windows
# Attempts to compile the Caro game using g++ directly, MSVC cl directly, or CMake.

$SourceFiles = @(
    "src/main.cpp",
    "src/Player.cpp",
    "src/PlayerManager.cpp",
    "src/Board.cpp",
    "src/Bot.cpp",
    "src/ReplayManager.cpp",
    "src/Game.cpp"
)

Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "Building Caro Game..." -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan

# Check for g++ (standard compiler in Dev-Cpp / MinGW)
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    Write-Host "Found g++. Compiling..." -ForegroundColor Yellow
    g++ -std=c++17 -O3 $SourceFiles -o CaroGame.exe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build Successful! Executable: CaroGame.exe" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Build failed using g++." -ForegroundColor Red
        exit 1
    }
}

# Check for cl (MSVC)
if (Get-Command cl -ErrorAction SilentlyContinue) {
    Write-Host "Found MSVC cl. Compiling..." -ForegroundColor Yellow
    cl /EHsc /std:c++17 $SourceFiles /Fe:CaroGame.exe
    if ($LASTEXITCODE -eq 0) {
        # Clean up intermediate files
        Remove-Item *.obj -ErrorAction SilentlyContinue
        Write-Host "Build Successful! Executable: CaroGame.exe" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Build failed using cl." -ForegroundColor Red
        exit 1
    }
}

# Check for cmake
if (Get-Command cmake -ErrorAction SilentlyContinue) {
    Write-Host "Found CMake. Generating build files..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Force -Path build | Out-Null
    Push-Location build
    cmake ..
    cmake --build . --config Release
    Pop-Location
    if (Test-Path "build/Release/CaroGame.exe") {
        Copy-Item "build/Release/CaroGame.exe" -Destination "CaroGame.exe"
        Write-Host "Build Successful via CMake! Executable: CaroGame.exe" -ForegroundColor Green
        exit 0
    } elseif (Test-Path "build/CaroGame.exe") {
        Copy-Item "build/CaroGame.exe" -Destination "CaroGame.exe"
        Write-Host "Build Successful via CMake! Executable: CaroGame.exe" -ForegroundColor Green
        exit 0
    } else {
        Write-Host "Build failed using CMake." -ForegroundColor Red
        exit 1
    }
}

Write-Host "Error: No suitable compiler (g++, cl, or cmake) was found in your PATH." -ForegroundColor Red
Write-Host "Please ensure Dev-Cpp, MinGW, or Visual Studio is installed and added to PATH." -ForegroundColor Red
exit 1
