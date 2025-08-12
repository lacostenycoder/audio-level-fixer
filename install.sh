#!/bin/bash

# Audio Processor - Installation Script
# This script installs the Audio Processor application

set -e

echo "Audio Processor - Installation Script"
echo "====================================="

# Check if we're running as root
if [ "$EUID" -eq 0 ]; then
    echo "Warning: Running as root. Installing system-wide."
    INSTALL_DIR="/opt/AudioProcessor"
    DESKTOP_DIR="/usr/share/applications"
    BIN_DIR="/usr/local/bin"
else
    echo "Installing for current user."
    INSTALL_DIR="$HOME/.local/share/AudioProcessor"
    DESKTOP_DIR="$HOME/.local/share/applications"
    BIN_DIR="$HOME/.local/bin"
fi

# Check if application is built
BUILD_EXECUTABLE="Build/AudioProcessor_artefacts/Audio Processor"
if [ ! -f "$BUILD_EXECUTABLE" ]; then
    echo "Error: Application not found. Please build first:"
    echo "  ./build_linux.sh"
    exit 1
fi

echo "Installing Audio Processor..."

# Create installation directory
mkdir -p "$INSTALL_DIR"
mkdir -p "$DESKTOP_DIR"
mkdir -p "$BIN_DIR"

# Copy application
echo "Copying application files..."
cp "$BUILD_EXECUTABLE" "$INSTALL_DIR/"

# Create desktop entry
echo "Creating desktop entry..."
cat > "$DESKTOP_DIR/audio-processor.desktop" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Audio Processor
Comment=Professional real-time audio processing
Exec=$INSTALL_DIR/Audio Processor
Icon=audio-card
Terminal=false
Categories=AudioVideo;Audio;
Keywords=audio;microphone;compression;limiter;processing;
StartupNotify=true
EOF

# Create command-line launcher
echo "Creating command-line launcher..."
cat > "$BIN_DIR/audio-processor" << EOF
#!/bin/bash
cd "$INSTALL_DIR"
exec "./Audio Processor" "\$@"
EOF

chmod +x "$BIN_DIR/audio-processor"

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    echo "Updating desktop database..."
    if [ "$EUID" -eq 0 ]; then
        update-desktop-database /usr/share/applications
    else
        update-desktop-database "$HOME/.local/share/applications"
    fi
fi

echo ""
echo "✅ Installation completed successfully!"
echo ""
echo "Application installed to: $INSTALL_DIR"
echo ""
echo "You can now run Audio Processor:"
echo "  - From applications menu: Search for 'Audio Processor'"
echo "  - From command line: audio-processor"
echo "  - Direct execution: '$INSTALL_DIR/Audio Processor'"
echo ""
echo "Note: Make sure JACK audio server is running for virtual audio functionality."
echo "Install JACK if not already installed: sudo apt install jackd2"
echo ""

# Check if user is in audio group
if ! groups | grep -q audio; then
    echo "Recommendation: Add your user to the audio group for better audio performance:"
    echo "  sudo usermod -a -G audio \$USER"
    echo "  (logout and login again after running this command)"
    echo ""
fi

echo "Enjoy using Audio Processor! 🎵"

