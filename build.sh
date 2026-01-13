#!/bin/bash

echo "====================================================="
echo "Blu-ray Remux to MKV CLI Tool - Unix Build Script"
echo "====================================================="
echo ""

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Please install CMake using your package manager."
    exit 1
fi

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo "Error: Git is not installed. Please install Git using your package manager."
    exit 1
fi

# Create build directory
mkdir -p build
cd build

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Linux*)
        echo "Detected Linux system"
        # Install dependencies
        if command -v apt &> /dev/null; then
            echo "Step 1: Installing dependencies using apt..."
            sudo apt update
            sudo apt install -y build-essential libavformat-dev libavcodec-dev libavutil-dev libavdevice-dev
        elif command -v yum &> /dev/null; then
            echo "Step 1: Installing dependencies using yum..."
            sudo yum install -y gcc gcc-c++ make libavformat-devel libavcodec-devel libavutil-devel libavdevice-devel
        elif command -v pacman &> /dev/null; then
            echo "Step 1: Installing dependencies using pacman..."
            sudo pacman -Syu --noconfirm base-devel ffmpeg
        else
            echo "Error: Unsupported package manager. Please install FFmpeg development libraries manually."
            exit 1
        fi
        ;;
    Darwin*)
        echo "Detected macOS system"
        # Install dependencies using Homebrew
        if command -v brew &> /dev/null; then
            echo "Step 1: Installing dependencies using Homebrew..."
            brew install ffmpeg
        else
            echo "Error: Homebrew is not installed. Please install Homebrew from https://brew.sh/"
            exit 1
        fi
        ;;
    *)
        echo "Error: Unsupported OS: ${OS}"
        exit 1
        ;;
esac

echo "Step 2: Generating Makefile..."
cmake ..

echo "Step 3: Building project..."
make -j$(nproc)

echo "Step 4: Running tests..."
if [ -f test/bdremux_tests ]; then
    echo "Running tests..."
    ./test/bdremux_tests
    if [ $? -ne 0 ]; then
        echo "Error: Tests failed!"
        exit 1
    fi
else
    echo "Warning: Test executable not found!"
fi

echo ""
echo "Build completed successfully!"
echo ""
echo "Main executable: $(pwd)/src/bdremux"
echo "Test executable: $(pwd)/test/bdremux_tests"
echo ""