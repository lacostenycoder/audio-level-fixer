# Audio Processor User Manual

## Table of Contents
1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Interface Overview](#interface-overview)
4. [Audio Processing Controls](#audio-processing-controls)
5. [Virtual Audio Setup](#virtual-audio-setup)
6. [Presets and Settings](#presets-and-settings)
7. [Troubleshooting](#troubleshooting)
8. [Advanced Usage](#advanced-usage)

## Introduction

Audio Processor is a professional real-time audio processing application designed to enhance microphone input before it reaches recording software. It provides the same high-quality audio processing found in professional DAWs like Ableton Live, but in a simple, dedicated application.

### What Audio Processor Does
- **Boosts** weak microphone signals
- **Compresses** dynamic range for consistent levels
- **Limits** peaks to prevent distortion
- **Routes** processed audio through a virtual cable to recording software

### Who Should Use Audio Processor
- **Podcasters** who want professional-sounding recordings
- **Streamers** who need consistent audio levels
- **Content Creators** recording voice-overs
- **Musicians** recording vocals or instruments
- **Anyone** who wants better microphone audio quality

## Getting Started

### System Requirements
- **Windows**: Windows 10 version 1903 or later
- **macOS**: macOS 10.15 (Catalina) or later  
- **Linux**: Ubuntu 18.04+ or equivalent distribution
- **RAM**: 4GB minimum, 8GB recommended
- **CPU**: Modern multi-core processor

### Installation

#### Quick Start (All Platforms)
1. Download the appropriate installer for your platform
2. Run the installer and follow the prompts
3. Launch Audio Processor from your applications menu
4. Grant microphone permissions when prompted

#### Detailed Installation Instructions

**Windows:**
1. Download `AudioProcessor-Windows-Setup.exe`
2. Right-click and select "Run as administrator"
3. Follow the installation wizard
4. Launch from Start Menu > Audio Processor

**macOS:**
1. Download `AudioProcessor-macOS.dmg`
2. Open the DMG file
3. Drag Audio Processor to Applications folder
4. Launch from Applications or Launchpad
5. Grant microphone access in System Preferences > Security & Privacy

**Linux:**
1. Download `AudioProcessor-Linux.tar.gz`
2. Extract: `tar -xzf AudioProcessor-Linux.tar.gz`
3. Run: `cd AudioProcessor && ./install.sh`
4. Launch: `audio-processor` or from applications menu

### First Launch Setup
1. **Grant Permissions**: Allow microphone access when prompted
2. **Select Audio Device**: Choose your microphone from the input dropdown
3. **Test Audio**: Speak into your microphone and watch the input meter
4. **Enable Processing**: Click the "Enable" button to start processing

## Interface Overview

Audio Processor features a clean, three-section interface designed for ease of use:

### Input Section (Left)
- **Input Device Selector**: Choose your microphone or audio interface
- **Input Gain**: Boost weak signals from -20dB to +40dB
- **Input Meter**: Real-time level display with peak indicators
- **Mute Button**: Temporarily disable input

### Processing Section (Center)
- **Enable/Disable**: Master switch for all audio processing
- **Compressor Controls**: Threshold, ratio, attack, and release
- **Limiter Controls**: Ceiling, lookahead, and release
- **Gain Reduction Meter**: Shows how much compression/limiting is applied
- **Preset Selector**: Quick access to optimized settings

### Output Section (Right)
- **Output Device Selector**: Choose where processed audio goes
- **Output Gain**: Final level adjustment from -20dB to +20dB
- **Output Meter**: Real-time level display of processed audio
- **Virtual Audio Toggle**: Enable/disable virtual audio cable

### Status Bar (Bottom)
- **Sample Rate**: Current audio sample rate
- **Buffer Size**: Audio buffer size and latency information
- **CPU Usage**: Real-time CPU usage indicator
- **Virtual Audio Status**: Shows if virtual audio cable is active

## Audio Processing Controls

### Input Gain
**Purpose**: Amplifies weak microphone signals before processing

**Range**: -20dB to +40dB
**Default**: 0dB
**Usage**: 
- Increase for quiet microphones or distant recording
- Decrease if input is too loud and causing distortion
- Watch the input meter - aim for peaks around -12dB to -6dB

### Compressor
**Purpose**: Reduces dynamic range, making quiet sounds louder and loud sounds quieter

#### Threshold
- **Range**: -40dB to 0dB
- **Default**: -20dB
- **Usage**: Set where compression begins. Lower values = more compression

#### Ratio
- **Range**: 1:1 to 10:1
- **Default**: 4:1
- **Usage**: How much compression is applied. Higher ratios = more aggressive compression

#### Attack
- **Range**: 0.1ms to 100ms
- **Default**: 1ms
- **Usage**: How quickly compression responds. Faster = more aggressive, slower = more natural

#### Release
- **Range**: 1ms to 1000ms
- **Default**: 30ms
- **Usage**: How quickly compression stops. Faster = more pumping, slower = smoother

### Limiter
**Purpose**: Prevents audio from exceeding a set level, protecting against distortion

#### Ceiling
- **Range**: -10dB to 0dB
- **Default**: -0.3dB
- **Usage**: Maximum output level. Set slightly below 0dB to prevent digital clipping

#### Lookahead
- **Range**: 0ms to 10ms
- **Default**: 3ms
- **Usage**: How far ahead the limiter looks. More lookahead = smoother limiting but higher latency

#### Release
- **Range**: 1ms to 1000ms
- **Default**: 300ms
- **Usage**: How quickly limiting recovers. Faster = more aggressive, slower = more transparent

### Output Gain
**Purpose**: Final level adjustment after all processing

**Range**: -20dB to +20dB
**Default**: 0dB
**Usage**: Fine-tune final output level to match your recording software's requirements

## Virtual Audio Setup

The virtual audio cable allows you to route processed audio to recording software as if it were a separate microphone.

### How It Works
1. Your microphone → Audio Processor (processing applied)
2. Audio Processor → Virtual Audio Cable
3. Virtual Audio Cable → Recording Software (OBS, Audacity, etc.)

### Platform-Specific Setup

#### Windows (WASAPI)
1. Enable virtual audio in Audio Processor
2. In your recording software, select "Audio Processor Virtual Device" as the audio input
3. The processed audio will appear as a virtual microphone

**Compatible Software**: OBS Studio, Audacity, Adobe Audition, Reaper, Discord, Zoom, Teams

#### macOS (Core Audio)
1. Enable virtual audio in Audio Processor
2. In your recording software, select "Audio Processor Virtual Device" from the input device list
3. The virtual device appears system-wide in all audio applications

**Compatible Software**: OBS Studio, Logic Pro, GarageBand, Audacity, QuickTime, FaceTime, Zoom

#### Linux (JACK)
1. Start JACK audio server: `jackd -d alsa` or use QjackCtl
2. Enable virtual audio in Audio Processor
3. Use JACK connection tools to route audio:
   ```bash
   # Connect your microphone to Audio Processor
   jack_connect system:capture_1 "Audio Processor:input"
   
   # Connect Audio Processor to your recording software
   jack_connect "Audio Processor:output" recording_app:input_1
   ```

**Compatible Software**: Ardour, Audacity (with JACK), OBS Studio (with JACK), Reaper

### Verification
To verify the virtual audio cable is working:
1. Enable virtual audio in Audio Processor
2. Speak into your microphone
3. Check that the output meter shows activity
4. In your recording software, select the virtual device and verify audio is received

## Presets and Settings

### Built-in Presets

#### Default
- **Use Case**: General purpose, balanced settings
- **Settings**: Moderate compression (4:1 ratio, -20dB threshold), gentle limiting
- **Best For**: Most users starting out

#### Podcast
- **Use Case**: Spoken word content with consistent levels
- **Settings**: Aggressive compression (6:1 ratio, -24dB threshold), tight limiting
- **Best For**: Podcast recording, voice-overs, audiobooks

#### Streaming
- **Use Case**: Live streaming with dynamic content
- **Settings**: Fast compression (2ms attack), quick limiting recovery
- **Best For**: Twitch/YouTube streaming, live commentary

#### Voice Over
- **Use Case**: Professional voice recording
- **Settings**: Transparent compression (3:1 ratio), minimal limiting
- **Best For**: Commercial voice-overs, professional recordings

### Custom Settings
1. Start with a preset that's close to your needs
2. Adjust individual controls while monitoring the meters
3. Save your settings using the "Save Preset" button
4. Name your preset for easy recall

### Settings File Location
- **Windows**: `%APPDATA%/AudioProcessor/settings.xml`
- **macOS**: `~/Library/Application Support/AudioProcessor/settings.xml`
- **Linux**: `~/.config/AudioProcessor/settings.xml`

## Troubleshooting

### Common Issues

#### No Audio Input
**Symptoms**: Input meter shows no activity
**Solutions**:
1. Check microphone is connected and working
2. Verify correct input device is selected
3. Check system audio permissions
4. Increase input gain
5. Test microphone in other applications

#### No Audio Output
**Symptoms**: Output meter shows no activity despite input
**Solutions**:
1. Check that processing is enabled
2. Verify output device is selected
3. Check output gain is not at minimum
4. Disable and re-enable virtual audio

#### Audio Dropouts/Crackling
**Symptoms**: Intermittent audio interruptions
**Solutions**:
1. Increase audio buffer size in system settings
2. Close unnecessary applications
3. Check CPU usage in Audio Processor
4. Update audio drivers
5. Restart Audio Processor

#### Virtual Audio Not Working
**Symptoms**: Recording software doesn't see virtual device
**Solutions**:
1. Restart Audio Processor as administrator (Windows)
2. Check audio permissions (macOS)
3. Verify JACK is running (Linux)
4. Restart recording software
5. Reboot computer

#### High CPU Usage
**Symptoms**: Computer becomes slow, audio dropouts
**Solutions**:
1. Reduce audio quality in system settings
2. Increase buffer size
3. Close other audio applications
4. Check for background processes
5. Consider hardware upgrade

### Platform-Specific Issues

#### Windows
- **WASAPI Errors**: Restart Windows Audio service
- **Permission Denied**: Run as administrator
- **No Virtual Device**: Check Windows audio drivers

#### macOS
- **Gatekeeper Blocking**: Allow in Security & Privacy settings
- **No Microphone Access**: Grant permission in System Preferences
- **Core Audio Errors**: Reset Core Audio: `sudo killall coreaudiod`

#### Linux
- **JACK Won't Start**: Check real-time permissions, add user to audio group
- **Permission Denied**: `sudo usermod -a -G audio $USER`
- **No Audio Devices**: Check ALSA/PulseAudio configuration

### Getting Help
1. Check this manual's troubleshooting section
2. Verify your system meets requirements
3. Try restarting the application
4. Check the project's issue tracker
5. Contact support with detailed system information

## Advanced Usage

### Professional Workflows

#### Podcast Production
1. Use "Podcast" preset as starting point
2. Set input gain to achieve -18dB to -12dB peaks
3. Adjust compressor threshold to catch most speech
4. Set limiter ceiling to -1dB for safety margin
5. Route to recording software via virtual audio cable

#### Live Streaming
1. Use "Streaming" preset for dynamic content
2. Monitor gain reduction meter - aim for 3-6dB average
3. Use faster attack/release for responsive processing
4. Test with various content types (speech, music, sound effects)
5. Set up backup recording in case of stream issues

#### Music Recording
1. Start with "Default" preset
2. Use gentler compression (2:1 to 3:1 ratio)
3. Set slower attack times to preserve transients
4. Adjust based on instrument type:
   - Vocals: Medium compression, fast limiter
   - Acoustic guitar: Light compression, slow release
   - Electric instruments: Varies by style

### Integration with DAWs

#### Using with Professional Software
Audio Processor can work alongside professional DAWs:
1. Use Audio Processor for live monitoring and streaming
2. Record dry signal in DAW for post-production flexibility
3. Use virtual audio cable for real-time collaboration
4. Apply different processing chains for different purposes

#### Comparison with DAW Processing
Audio Processor provides:
- **Lower latency** than most DAW plugins
- **Dedicated interface** for live use
- **System-wide availability** via virtual audio cable
- **Simplified controls** for non-technical users

### Performance Optimization

#### Reducing Latency
1. Use smallest stable buffer size
2. Close unnecessary applications
3. Disable system audio enhancements
4. Use dedicated audio interface if possible
5. Consider real-time operating system tweaks

#### CPU Optimization
1. Monitor CPU usage in status bar
2. Reduce lookahead time if not needed
3. Use simpler processing chains when possible
4. Close other audio applications
5. Consider hardware upgrade for demanding use

### Automation and Control

#### MIDI Control (Future Feature)
Audio Processor is designed to support MIDI control for:
- Preset switching
- Parameter adjustment
- Mute/bypass controls
- Real-time automation

#### API Integration (Future Feature)
Planned features include:
- REST API for remote control
- Plugin versions for DAW integration
- Command-line interface for automation
- Integration with streaming software

---

This manual covers the essential features and usage of Audio Processor. For the latest updates and additional resources, visit the project repository or support website.

