@echo off
setlocal enabledelayedexpansion

echo =====================================================
echo Blu-ray Remux to MKV CLI Tool - Windows Build Script
echo =====================================================
echo.

:: Check if CMake is installed
cmake --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: CMake is not installed. Please install CMake from https://cmake.org/download/
    exit /b 1
)

:: Check if Git is installed
git --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: Git is not installed. Please install Git from https://git-scm.com/download/win
    exit /b 1
)

:: Create build directory
mkdir build 2>nul
cd build

echo Step 1: Installing vcpkg...
if not exist vcpkg (
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    call bootstrap-vcpkg.bat
    cd ..
)

echo Step 2: Installing FFmpeg dependencies...
vcpkg\vcpkg install ffmpeg[core]:x64-windows

echo Step 3: Generating Visual Studio project...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake

echo Step 4: Building project...
cmake --build . --config Release

echo Step 5: Running tests...
if exist test\Release\bdremux_tests.exe (
    echo Running tests...
    test\Release\bdremux_tests.exe
    if %ERRORLEVEL% neq 0 (
        echo Error: Tests failed!
        exit /b 1
    )
) else (
    echo Warning: Test executable not found!
)

echo.
echo Build completed successfully!
echo.
echo Main executable: %CD%\src\Release\bdremux.exe
echo Test executable: %CD%\test\Release\bdremux_tests.exe
echo.

endlocal