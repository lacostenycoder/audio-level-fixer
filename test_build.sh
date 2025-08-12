#!/bin/bash

echo "Audio Processor - Build Test Script"
echo "==================================="

# Check for CMake
if command -v cmake &> /dev/null; then
    echo "✓ CMake found: $(cmake --version | head -n1)"
else
    echo "✗ CMake not found. Please install CMake."
    exit 1
fi

# Check for build tools
if command -v make &> /dev/null || command -v ninja &> /dev/null; then
    echo "✓ Build tools available"
else
    echo "✗ No build tools found (make or ninja required)"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
BUILD_DIR="Build"
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir "$BUILD_DIR"
cd "$BUILD_DIR"

echo "✓ Build directory created"

# Configure with CMake
echo "Configuring build with CMake..."
if cmake .. -DCMAKE_BUILD_TYPE=Release; then
    echo "✓ CMake configuration completed"
else
    echo "✗ CMake configuration failed"
    exit 1
fi

# Build the application
echo "Building Audio Processor..."
if cmake --build . --config Release; then
    echo "✓ Build completed successfully"
else
    echo "✗ Build failed"
    exit 1
fi

echo ""
echo "🎉 Build test completed!"
echo ""
echo "Next steps for Windows:"
echo "1. Install Visual Studio 2019+ or Build Tools"
echo "2. Run build_windows.bat on Windows"
echo "3. Use build_installer.bat to create installer"