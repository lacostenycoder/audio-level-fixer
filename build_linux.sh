#!/bin/bash

# Audio Processor - Linux Build Script
# This script builds the Audio Processor application for Linux

set -e

echo "Audio Processor - Linux Build Script"
echo "===================================="

# Check for required dependencies
echo "Checking dependencies..."

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is required but not installed."
    echo "Install with: sudo apt install cmake"
    exit 1
fi

# Check for build tools
if ! command -v make &> /dev/null; then
    echo "Error: Make is required but not installed."
    echo "Install with: sudo apt install build-essential"
    exit 1
fi

# Check for JACK development libraries
if ! pkg-config --exists jack; then
    echo "Error: JACK development libraries are required but not installed."
    echo "Install with: sudo apt install libjack-jackd2-dev jackd2"
    exit 1
fi

# Check for other required libraries
echo "Checking for required system libraries..."
REQUIRED_PACKAGES=(
    "alsa"
    "freetype2"
    "fontconfig"
    "gl"
    "libcurl"
    "webkit2gtk-4.0"
    "gtk+-x11-3.0"
)

for package in "${REQUIRED_PACKAGES[@]}"; do
    if ! pkg-config --exists "$package"; then
        echo "Warning: $package not found. Some features may not work."
    fi
done

echo "✓ Dependencies check completed"

# Create build directory
echo "Setting up build environment..."
BUILD_DIR="Build"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "✓ Build directory created"

# Configure with CMake
echo "Configuring build with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

echo "✓ CMake configuration completed"

# Build the application
echo "Building Audio Processor..."
make -j$(nproc)

echo "✓ Build completed successfully"

# Check if executable was created
EXECUTABLE_PATH="AudioProcessor_artefacts/Audio Processor"
if [ -f "$EXECUTABLE_PATH" ]; then
    echo "✓ Executable created: $EXECUTABLE_PATH"
    
    # Get file size
    FILE_SIZE=$(du -h "$EXECUTABLE_PATH" | cut -f1)
    echo "  File size: $FILE_SIZE"
    
    # Check if it's executable
    if [ -x "$EXECUTABLE_PATH" ]; then
        echo "  Permissions: Executable"
    else
        echo "  Warning: File is not executable"
    fi
else
    echo "Error: Executable not found at expected location"
    exit 1
fi

echo ""
echo "🎉 Build completed successfully!"
echo ""
echo "Application location: $PWD/$EXECUTABLE_PATH"
echo ""
echo "To run the application:"
echo "  cd $PWD"
echo "  ./'$EXECUTABLE_PATH'"
echo ""
echo "Note: Make sure JACK audio server is running for virtual audio functionality."
echo "You can start JACK with: jackd -d alsa"
echo ""
echo "For GUI applications, you may need to run in a desktop environment."

