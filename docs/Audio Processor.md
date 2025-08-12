# Audio Processor

A cross-platform audio processing application that provides real-time microphone boost, compression, and brick-wall limiting before the signal reaches recording software, with integrated virtual audio cable functionality.

## Features

### 🎵 Professional Audio Processing
- **Input Gain Boost**: Adjustable input gain from -20dB to +40dB
- **Dynamic Range Compression**: RMS-based compressor with configurable threshold, ratio, attack, and release
- **Brick-Wall Limiter**: Advanced lookahead limiter with 3ms lookahead and adjustable ceiling
- **Real-Time Processing**: Sub-20ms total latency for live applications

### 🎛️ User Interface
- **Three-Section Layout**: Input, Processing, and Output controls
- **Real-Time Meters**: Input level, output level, and gain reduction visualization
- **Built-in Presets**: Podcast, Streaming, Voice Over, and Default configurations
- **Audio Device Selection**: Choose input and output devices
- **Cross-Platform GUI**: Native look and feel on Windows, macOS, and Linux

### 🔌 Virtual Audio Cable
- **Integrated Virtual Audio**: No need for separate virtual audio cable software
- **Platform-Specific Implementation**:
  - **Linux**: JACK Audio Connection Kit integration
  - **Windows**: WASAPI virtual audio endpoint (framework ready)
  - **macOS**: Core Audio HAL plugin (framework ready)

## System Requirements

### All Platforms
- **CPU**: Modern multi-core processor (Intel/AMD x64 or Apple Silicon)
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 100MB free space

### Linux
- **OS**: Ubuntu 18.04+ or equivalent distribution
- **Audio**: JACK Audio Connection Kit or ALSA
- **Dependencies**: See [Linux Build Requirements](#linux)

### Windows
- **OS**: Windows 10 version 1903 or later
- **Audio**: WASAPI-compatible audio system
- **Build Tools**: Visual Studio 2019+ or Build Tools for Visual Studio

### macOS
- **OS**: macOS 10.15 (Catalina) or later
- **Audio**: Core Audio (built-in)
- **Build Tools**: Xcode Command Line Tools

## Installation

### Pre-built Binaries
*Coming soon - pre-built binaries will be available for download*

### Building from Source

#### Linux

1. **Install Dependencies**:
   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake libjack-jackd2-dev jackd2
   sudo apt install -y libasound2-dev libfreetype6-dev libfontconfig1-dev
   sudo apt install -y libgl1-mesa-dev libcurl4-openssl-dev
   ```

2. **Clone and Build**:
   ```bash
   git clone <repository-url>
   cd AudioProcessor
   ./build_linux.sh
   ```

3. **Run**:
   ```bash
   cd Build
   ./AudioProcessor_artefacts/"Audio Processor"
   ```

#### Windows

1. **Install Prerequisites**:
   - Visual Studio 2019 or later with C++ development tools
   - CMake 3.15 or later

2. **Build**:
   ```cmd
   git clone <repository-url>
   cd AudioProcessor
   build_windows.bat
   ```

3. **Run**:
   ```cmd
   cd Build
   AudioProcessor_artefacts\Release\"Audio Processor.exe"
   ```

#### macOS

1. **Install Prerequisites**:
   ```bash
   xcode-select --install
   brew install cmake  # or download from cmake.org
   ```

2. **Build**:
   ```bash
   git clone <repository-url>
   cd AudioProcessor
   ./build_macos.sh
   ```

3. **Run**:
   ```bash
   cd Build
   open AudioProcessor_artefacts/"Audio Processor.app"
   ```

## Usage

### Basic Setup

1. **Launch the Application**: Start Audio Processor using the method for your platform
2. **Select Audio Devices**: Choose your microphone as input and desired output
3. **Configure Processing**: Adjust gain, compression, and limiting settings
4. **Enable Virtual Audio**: Activate the virtual audio device for routing

### Virtual Audio Routing

#### Linux (JACK)
1. Start JACK server: `jackd -d alsa`
2. Launch Audio Processor
3. Use `qjackctl` or `jack_connect` to route audio:
   ```bash
   # Connect microphone to Audio Processor input
   jack_connect system:capture_1 "Audio Processor Virtual Device:input"
   
   # Connect Audio Processor output to recording software
   jack_connect "Audio Processor Virtual Device:output" recording_app:input_1
   ```

#### Windows (WASAPI)
1. Launch Audio Processor
2. In recording software (e.g., OBS, Audacity), select "Audio Processor Virtual Device" as input
3. The processed audio will appear as a virtual microphone

#### macOS (Core Audio)
1. Launch Audio Processor
2. In recording software, select "Audio Processor Virtual Device" from audio input devices
3. The processed audio will be available system-wide

### Presets

- **Default**: Balanced settings for general use
- **Podcast**: Optimized for spoken word content
- **Streaming**: Settings for live streaming applications
- **Voice Over**: Professional voice recording settings

### Controls

#### Input Section
- **Input Gain**: -20dB to +40dB boost
- **Input Meter**: Real-time input level display

#### Processing Section
- **Compressor**:
  - Threshold: -40dB to 0dB
  - Ratio: 1:1 to 10:1
  - Attack: 0.1ms to 100ms
  - Release: 1ms to 1000ms
- **Limiter**:
  - Ceiling: -10dB to 0dB
  - Lookahead: 0ms to 10ms
  - Release: 1ms to 1000ms

#### Output Section
- **Output Gain**: -20dB to +20dB
- **Output Meter**: Real-time output level display
- **Gain Reduction Meter**: Shows compression/limiting activity

## Technical Details

### Audio Processing Chain
1. **Input Gain**: Adjustable pre-amplification
2. **Compression**: RMS-based dynamic range compression
3. **Limiting**: Lookahead brick-wall limiting
4. **Output Gain**: Final level adjustment

### Performance
- **Latency**: <20ms total processing latency
- **CPU Usage**: Optimized for real-time performance
- **Sample Rates**: 44.1kHz, 48kHz, 96kHz support
- **Bit Depth**: 16-bit, 24-bit, 32-bit float

### Architecture
- **Framework**: JUCE (Jules' Utility Class Extensions)
- **Language**: C++17
- **Build System**: CMake
- **Audio APIs**:
  - Linux: JACK, ALSA
  - Windows: WASAPI, DirectSound
  - macOS: Core Audio

## Troubleshooting

### Linux
- **JACK not starting**: Check audio permissions and real-time settings
- **No audio devices**: Ensure ALSA/PulseAudio is properly configured
- **Permission denied**: Add user to `audio` group: `sudo usermod -a -G audio $USER`

### Windows
- **No virtual device**: Restart application as administrator
- **Audio dropouts**: Increase buffer size in audio settings
- **WASAPI errors**: Check Windows audio service is running

### macOS
- **App won't start**: Check Gatekeeper settings in Security & Privacy
- **No audio permission**: Grant microphone access in System Preferences
- **Core Audio errors**: Reset Core Audio: `sudo killall coreaudiod`

## Development

### Project Structure
```
AudioProcessor/
├── Source/                 # Application source code
│   ├── Main.cpp           # Application entry point
│   ├── MainComponent.*    # Main GUI component
│   ├── AudioEngine.*     # Audio processing engine
│   ├── Compressor.*      # Compression algorithm
│   ├── Limiter.*         # Limiting algorithm
│   └── VirtualAudioDevice.* # Virtual audio device
├── Build/                 # Build output directory
├── CMakeLists.txt        # CMake configuration
├── build_*.sh/bat        # Platform-specific build scripts
└── README.md             # This file
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on target platforms
5. Submit a pull request

### Dependencies
- **JUCE**: Audio application framework
- **CMake**: Build system
- **Platform-specific**:
  - Linux: JACK development libraries
  - Windows: Windows SDK
  - macOS: Xcode Command Line Tools

## License

This project is licensed under the GPL-3.0 License - see the LICENSE file for details.

## Acknowledgments

- **JUCE Framework**: For providing excellent cross-platform audio capabilities
- **JACK Audio**: For professional audio routing on Linux
- **VB-Audio**: For inspiration from their virtual cable implementations
- **Ableton Live**: For reference implementation of professional audio processing

## Support

For issues, questions, or contributions:
1. Check the [Troubleshooting](#troubleshooting) section
2. Search existing issues in the repository
3. Create a new issue with detailed information
4. Include system information and steps to reproduce

---

**Audio Processor** - Professional real-time audio processing for content creators, podcasters, and streamers.

