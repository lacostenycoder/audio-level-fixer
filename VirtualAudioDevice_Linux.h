#pragma once

#include <JuceHeader.h>
#include <jack/jack.h>
#include <memory>
#include <atomic>

//==============================================================================
class VirtualAudioDevice_Linux
{
public:
    VirtualAudioDevice_Linux();
    ~VirtualAudioDevice_Linux();
    
    bool initialize(const juce::String& deviceName);
    void shutdown();
    
    bool isAvailable() const { return jackClient != nullptr; }
    bool isActive() const { return active.load(); }
    
    void setActive(bool shouldBeActive);
    void processAudioBlock(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    
    // JACK callbacks
    static int jackProcessCallback(jack_nframes_t nframes, void* arg);
    static void jackShutdownCallback(void* arg);
    static int jackSampleRateCallback(jack_nframes_t nframes, void* arg);
    static int jackBufferSizeCallback(jack_nframes_t nframes, void* arg);
    
private:
    // JACK client and ports
    jack_client_t* jackClient = nullptr;
    jack_port_t* inputPort = nullptr;
    jack_port_t* outputPort = nullptr;
    
    // Audio buffer for inter-thread communication
    juce::AudioBuffer<float> audioBuffer;
    juce::AbstractFifo audioFifo;
    std::unique_ptr<float[]> fifoBuffer;
    
    // State
    std::atomic<bool> active{false};
    std::atomic<bool> initialized{false};
    
    // Audio parameters
    int sampleRate = 44100;
    int bufferSize = 512;
    
    // Thread safety
    juce::CriticalSection bufferLock;
    
    // Helper methods
    bool createJackClient(const juce::String& clientName);
    bool createJackPorts();
    bool activateJackClient();
    void deactivateJackClient();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VirtualAudioDevice_Linux)
};

