#include "VirtualAudioDevice_Linux.h"

//==============================================================================
VirtualAudioDevice_Linux::VirtualAudioDevice_Linux()
    : audioFifo(2048) // 2048 sample FIFO buffer
{
    fifoBuffer = std::make_unique<float[]>(2048 * 2); // Stereo buffer
    audioBuffer.setSize(2, 512); // Default buffer size
}

VirtualAudioDevice_Linux::~VirtualAudioDevice_Linux()
{
    shutdown();
}

bool VirtualAudioDevice_Linux::initialize(const juce::String& deviceName)
{
    if (initialized.load())
        return true;
    
    DBG("Initializing Linux virtual audio device: " + deviceName);
    
    // Create JACK client
    if (!createJackClient(deviceName))
    {
        DBG("Failed to create JACK client");
        return false;
    }
    
    // Set up JACK callbacks
    jack_set_process_callback(jackClient, jackProcessCallback, this);
    jack_on_shutdown(jackClient, jackShutdownCallback, this);
    jack_set_sample_rate_callback(jackClient, jackSampleRateCallback, this);
    jack_set_buffer_size_callback(jackClient, jackBufferSizeCallback, this);
    
    // Create input and output ports
    if (!createJackPorts())
    {
        DBG("Failed to create JACK ports");
        shutdown();
        return false;
    }
    
    // Get JACK audio parameters
    sampleRate = jack_get_sample_rate(jackClient);
    bufferSize = jack_get_buffer_size(jackClient);
    
    DBG("JACK parameters: " + juce::String(sampleRate) + " Hz, " + juce::String(bufferSize) + " samples");
    
    // Activate JACK client
    if (!activateJackClient())
    {
        DBG("Failed to activate JACK client");
        shutdown();
        return false;
    }
    
    initialized.store(true);
    DBG("Linux virtual audio device initialized successfully");
    return true;
}

void VirtualAudioDevice_Linux::shutdown()
{
    if (!initialized.load())
        return;
    
    DBG("Shutting down Linux virtual audio device");
    
    setActive(false);
    deactivateJackClient();
    
    if (jackClient)
    {
        jack_client_close(jackClient);
        jackClient = nullptr;
    }
    
    inputPort = nullptr;
    outputPort = nullptr;
    initialized.store(false);
    
    DBG("Linux virtual audio device shut down");
}

void VirtualAudioDevice_Linux::setActive(bool shouldBeActive)
{
    active.store(shouldBeActive);
    DBG("Linux virtual audio device " + juce::String(shouldBeActive ? "activated" : "deactivated"));
}

void VirtualAudioDevice_Linux::processAudioBlock(const juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!active.load() || !initialized.load())
        return;
    
    // Write audio data to FIFO for JACK thread to consume
    juce::ScopedLock lock(bufferLock);
    
    const int numChannels = juce::jmin(buffer.getNumChannels(), 2);
    const int availableSpace = audioFifo.getFreeSpace();
    const int samplesToWrite = juce::jmin(numSamples, availableSpace);
    
    if (samplesToWrite > 0)
    {
        int start1, size1, start2, size2;
        audioFifo.prepareToWrite(samplesToWrite, start1, size1, start2, size2);
        
        // Interleave audio data into FIFO buffer
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float* channelData = buffer.getReadPointer(channel, startSample);
            
            // Write first block
            for (int i = 0; i < size1; ++i)
            {
                fifoBuffer[(start1 + i) * 2 + channel] = channelData[i];
            }
            
            // Write second block (if wrapping)
            for (int i = 0; i < size2; ++i)
            {
                fifoBuffer[(start2 + i) * 2 + channel] = channelData[size1 + i];
            }
        }
        
        // Fill missing channel with zeros if mono input
        if (numChannels == 1)
        {
            for (int i = 0; i < size1; ++i)
            {
                fifoBuffer[(start1 + i) * 2 + 1] = 0.0f;
            }
            for (int i = 0; i < size2; ++i)
            {
                fifoBuffer[(start2 + i) * 2 + 1] = 0.0f;
            }
        }
        
        audioFifo.finishedWrite(samplesToWrite);
    }
}

//==============================================================================
// JACK Callbacks

int VirtualAudioDevice_Linux::jackProcessCallback(jack_nframes_t nframes, void* arg)
{
    auto* device = static_cast<VirtualAudioDevice_Linux*>(arg);
    
    if (!device->active.load())
        return 0;
    
    // Get JACK port buffers
    auto* inputBuffer = static_cast<jack_default_audio_sample_t*>(
        jack_port_get_buffer(device->inputPort, nframes));
    auto* outputBuffer = static_cast<jack_default_audio_sample_t*>(
        jack_port_get_buffer(device->outputPort, nframes));
    
    // Clear output buffer
    std::memset(outputBuffer, 0, nframes * sizeof(jack_default_audio_sample_t));
    
    // Read audio data from FIFO
    juce::ScopedLock lock(device->bufferLock);
    
    const int availableData = device->audioFifo.getNumReady();
    const int samplesToRead = juce::jmin(static_cast<int>(nframes), availableData);
    
    if (samplesToRead > 0)
    {
        int start1, size1, start2, size2;
        device->audioFifo.prepareToRead(samplesToRead, start1, size1, start2, size2);
        
        // De-interleave audio data from FIFO buffer to JACK output
        for (int i = 0; i < size1; ++i)
        {
            outputBuffer[i] = device->fifoBuffer[(start1 + i) * 2]; // Left channel
        }
        for (int i = 0; i < size2; ++i)
        {
            outputBuffer[size1 + i] = device->fifoBuffer[(start2 + i) * 2]; // Left channel
        }
        
        device->audioFifo.finishedRead(samplesToRead);
    }
    
    return 0;
}

void VirtualAudioDevice_Linux::jackShutdownCallback(void* arg)
{
    auto* device = static_cast<VirtualAudioDevice_Linux*>(arg);
    DBG("JACK server shutdown");
    device->initialized.store(false);
    device->active.store(false);
}

int VirtualAudioDevice_Linux::jackSampleRateCallback(jack_nframes_t nframes, void* arg)
{
    auto* device = static_cast<VirtualAudioDevice_Linux*>(arg);
    device->sampleRate = static_cast<int>(nframes);
    DBG("JACK sample rate changed to: " + juce::String(nframes));
    return 0;
}

int VirtualAudioDevice_Linux::jackBufferSizeCallback(jack_nframes_t nframes, void* arg)
{
    auto* device = static_cast<VirtualAudioDevice_Linux*>(arg);
    device->bufferSize = static_cast<int>(nframes);
    DBG("JACK buffer size changed to: " + juce::String(nframes));
    return 0;
}

//==============================================================================
// Helper Methods

bool VirtualAudioDevice_Linux::createJackClient(const juce::String& clientName)
{
    jack_status_t status;
    jackClient = jack_client_open(clientName.toUTF8(), JackNullOption, &status);
    
    if (!jackClient)
    {
        DBG("Failed to create JACK client. Status: " + juce::String(status));
        return false;
    }
    
    if (status & JackServerStarted)
    {
        DBG("JACK server started");
    }
    
    if (status & JackNameNotUnique)
    {
        DBG("JACK client name was not unique, assigned: " + juce::String(jack_get_client_name(jackClient)));
    }
    
    return true;
}

bool VirtualAudioDevice_Linux::createJackPorts()
{
    // Create input port (for receiving audio from our application)
    inputPort = jack_port_register(jackClient, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    if (!inputPort)
    {
        DBG("Failed to create JACK input port");
        return false;
    }
    
    // Create output port (for sending processed audio to other applications)
    outputPort = jack_port_register(jackClient, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    if (!outputPort)
    {
        DBG("Failed to create JACK output port");
        return false;
    }
    
    DBG("JACK ports created successfully");
    return true;
}

bool VirtualAudioDevice_Linux::activateJackClient()
{
    if (jack_activate(jackClient) != 0)
    {
        DBG("Failed to activate JACK client");
        return false;
    }
    
    DBG("JACK client activated");
    return true;
}

void VirtualAudioDevice_Linux::deactivateJackClient()
{
    if (jackClient)
    {
        jack_deactivate(jackClient);
        DBG("JACK client deactivated");
    }
}

