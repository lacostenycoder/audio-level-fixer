#!/bin/bash

# Audio Processor - macOS Build Script
# This script builds the Audio Processor application for macOS

set -e

echo "Audio Processor - macOS Build Script"
echo "===================================="

# Check for required dependencies
echo "Checking dependencies..."

# Check for Xcode Command Line Tools
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode Command Line Tools are required but not installed."
    echo "Install with: xcode-select --install"
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is required but not installed."
    echo "Install with: brew install cmake"
    echo "Or download from: https://cmake.org/download/"
    exit 1
fi

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
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15

echo "✓ CMake configuration completed"

# Build the application
echo "Building Audio Processor..."
make -j$(sysctl -n hw.ncpu)

echo "✓ Build completed successfully"

# Check if app bundle was created
APP_BUNDLE_PATH="AudioProcessor_artefacts/Audio Processor.app"
if [ -d "$APP_BUNDLE_PATH" ]; then
    echo "✓ App bundle created: $APP_BUNDLE_PATH"
    
    # Get bundle size
    BUNDLE_SIZE=$(du -sh "$APP_BUNDLE_PATH" | cut -f1)
    echo "  Bundle size: $BUNDLE_SIZE"
    
    # Check executable inside bundle
    EXECUTABLE_PATH="$APP_BUNDLE_PATH/Contents/MacOS/Audio Processor"
    if [ -f "$EXECUTABLE_PATH" ]; then
        echo "  Executable: Found"
        if [ -x "$EXECUTABLE_PATH" ]; then
            echo "  Permissions: Executable"
        else
            echo "  Warning: Executable is not executable"
        fi
    else
        echo "  Warning: Executable not found inside bundle"
    fi
else
    echo "Error: App bundle not found at expected location"
    exit 1
fi

echo ""
echo "🎉 Build completed successfully!"
echo ""
echo "Application location: $PWD/$APP_BUNDLE_PATH"
echo ""
echo "To run the application:"
echo "  open '$PWD/$APP_BUNDLE_PATH'"
echo ""
echo "Or from command line:"
echo "  '$PWD/$EXECUTABLE_PATH'"
echo ""
echo "Note: The application includes Core Audio virtual device support."
echo "For virtual audio functionality, the app may need to be signed and"
echo "granted appropriate permissions in System Preferences > Security & Privacy."
echo ""
echo "To create a distributable package:"
echo "  1. Sign the application with your Developer ID"
echo "  2. Create a DMG file for distribution"
echo "  3. Notarize the application for Gatekeeper compatibility"

