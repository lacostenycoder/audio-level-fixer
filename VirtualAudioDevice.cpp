#include "VirtualAudioDevice.h"

//==============================================================================
VirtualAudioDevice::VirtualAudioDevice()
{
    initializePlatformSpecific();
}

VirtualAudioDevice::~VirtualAudioDevice()
{
    shutdownPlatformSpecific();
}

void VirtualAudioDevice::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    DBG("VirtualAudioDevice prepared: " + juce::String(sampleRate) + " Hz, " + juce::String(samplesPerBlock) + " samples");
}

void VirtualAudioDevice::processAudioBlock(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!isActive() || !isAvailable())
        return;
    
#if JUCE_LINUX
    if (linuxDevice)
        linuxDevice->processAudioBlock(buffer, startSample, numSamples);
#elif JUCE_WINDOWS
    // Windows implementation would go here
#elif JUCE_MAC
    // macOS implementation would go here
#endif
}

void VirtualAudioDevice::releaseResources()
{
    setActive(false);
    DBG("VirtualAudioDevice resources released");
}

bool VirtualAudioDevice::isAvailable() const
{
#if JUCE_LINUX
    return linuxDevice && linuxDevice->isAvailable();
#elif JUCE_WINDOWS
    // Windows implementation would return availability status
    return false;
#elif JUCE_MAC
    // macOS implementation would return availability status
    return false;
#else
    return false;
#endif
}

bool VirtualAudioDevice::isActive() const
{
#if JUCE_LINUX
    return linuxDevice && linuxDevice->isActive();
#elif JUCE_WINDOWS
    // Windows implementation would return active status
    return false;
#elif JUCE_MAC
    // macOS implementation would return active status
    return false;
#else
    return false;
#endif
}

void VirtualAudioDevice::setActive(bool shouldBeActive)
{
#if JUCE_LINUX
    if (linuxDevice)
        linuxDevice->setActive(shouldBeActive);
#elif JUCE_WINDOWS
    // Windows implementation would set active status
#elif JUCE_MAC
    // macOS implementation would set active status
#endif
    
    DBG("VirtualAudioDevice " + juce::String(shouldBeActive ? "activated" : "deactivated"));
}

//==============================================================================
// Platform-specific implementations

void VirtualAudioDevice::initializePlatformSpecific()
{
#if JUCE_LINUX
    DBG("Initializing Linux virtual audio device");
    linuxDevice = std::make_unique<VirtualAudioDevice_Linux>();
    
    if (linuxDevice->initialize(deviceName))
    {
        DBG("Linux virtual audio device initialized successfully");
    }
    else
    {
        DBG("Failed to initialize Linux virtual audio device");
        linuxDevice.reset();
    }
    
#elif JUCE_WINDOWS
    // Windows WASAPI virtual device initialization
    DBG("Initializing Windows virtual audio device");
    // Implementation would go here
    
#elif JUCE_MAC
    // macOS Core Audio virtual device initialization
    DBG("Initializing macOS virtual audio device");
    // Implementation would go here
    
#else
    DBG("Virtual audio device not supported on this platform");
#endif
}

void VirtualAudioDevice::shutdownPlatformSpecific()
{
#if JUCE_LINUX
    if (linuxDevice)
    {
        DBG("Shutting down Linux virtual audio device");
        linuxDevice->shutdown();
        linuxDevice.reset();
    }
    
#elif JUCE_WINDOWS
    // Windows cleanup
    DBG("Shutting down Windows virtual audio device");
    
#elif JUCE_MAC
    // macOS cleanup
    DBG("Shutting down macOS virtual audio device");
    
#endif
}

