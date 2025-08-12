# Audio Processor

A real-time audio processing application with virtual audio device support for Windows and Linux. This application provides audio enhancement through compression and limiting, with preset configurations for different use cases.

⚠️ **Note: This application is currently in development and not fully functional.**

## Features

- **Real-time Audio Processing**: Dynamic range compression and limiting
- **Virtual Audio Device**: System-level audio routing for Windows and Linux
- **Preset Configurations**: 
  - Default: General purpose audio enhancement
  - Podcast: Optimized for voice recording
  - Streaming: Broadcast-ready audio processing
  - VoiceOver: Professional voice work settings
- **Cross-platform Support**: Windows, Linux, and macOS build scripts
- **Installer Generation**: Automated installer creation for Windows

## Project Structure

```
├── AudioEngine.cpp/h          # Core audio processing engine
├── Compressor.cpp/h           # Dynamic range compressor
├── Limiter.cpp/h              # Audio limiter
├── VirtualAudioDevice.cpp/h   # Cross-platform virtual audio device
├── VirtualAudioDevice_Linux.cpp/h  # Linux-specific implementation
├── MainComponent.cpp/h        # GUI main component
├── Main.cpp                   # Application entry point
├── *.preset                   # Audio processing presets
├── CMakeLists.txt             # Build configuration
├── build_*.sh/bat             # Platform-specific build scripts
└── docs/                      # Documentation
```

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Platform-specific audio libraries

### Windows

```bash
./build_windows.bat
```

### Linux

```bash
./build_linux.sh
```

### macOS

```bash
./build_macos.sh
```

## Installation

### Windows Installer

1. Build the application using the Windows build script
2. Run the installer creation script:
   ```bash
   ./build_installer.bat
   ```
3. Optionally sign the installer:
   ```bash
   ./sign_installer.bat
   ```

### Linux

```bash
./install.sh
```

## Testing

- `simple_test.bat` - Basic functionality test
- `test_build.sh` - Build verification test
- `test_application.cpp` - Unit tests
- `test_installer.bat` - Installer verification

## Development Status

This project is currently under development. The core audio processing components are implemented but the application may not be fully functional. Key areas still in development:

- Virtual audio device integration
- GUI implementation
- Cross-platform compatibility
- Audio routing and device management

## License

See `LICENSE.txt` for license information.

## Contributing

This project is in active development. Please refer to the documentation in the `docs/` directory for technical specifications and implementation details.