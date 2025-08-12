#pragma once

#include <JuceHeader.h>

#if JUCE_LINUX
#include "VirtualAudioDevice_Linux.h"
#endif

//==============================================================================
class VirtualAudioDevice
{
public:
    VirtualAudioDevice();
    ~VirtualAudioDevice();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processAudioBlock(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void releaseResources();
    
    // Device management
    bool isAvailable() const;
    juce::String getDeviceName() const { return deviceName; }
    
    // Status
    bool isActive() const;
    void setActive(bool shouldBeActive);
    
private:
    // Device state
    juce::String deviceName = "Audio Processor Virtual Device";
    
    // Audio parameters
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    
    // Platform-specific implementation
    void initializePlatformSpecific();
    void shutdownPlatformSpecific();
    
#if JUCE_LINUX
    std::unique_ptr<VirtualAudioDevice_Linux> linuxDevice;
#elif JUCE_WINDOWS
    // Windows WASAPI implementation (to be implemented)
    // std::unique_ptr<VirtualAudioDevice_Windows> windowsDevice;
#elif JUCE_MAC
    // macOS Core Audio implementation (to be implemented)
    // std::unique_ptr<VirtualAudioDevice_Mac> macDevice;
#endif
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VirtualAudioDevice)
};

