#include "AudioEngine.h"

//==============================================================================
AudioEngine::AudioEngine()
{
    // Initialize virtual audio device
    virtualDevice = std::make_unique<VirtualAudioDevice>();
}

AudioEngine::~AudioEngine()
{
    releaseResources();
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlockExpected;
    
    // Prepare smoothed values
    inputGain.reset(sampleRate, 0.05); // 50ms smoothing
    outputGain.reset(sampleRate, 0.05);
    inputLevelSmoothed.reset(sampleRate, 0.1); // 100ms smoothing for meters
    outputLevelSmoothed.reset(sampleRate, 0.1);
    
    // Prepare processing components
    compressor.prepareToPlay(sampleRate, samplesPerBlockExpected);
    limiter.prepareToPlay(sampleRate, samplesPerBlockExpected);
    
    // Prepare virtual audio device
    virtualDevice->prepareToPlay(sampleRate, samplesPerBlockExpected);
}

void AudioEngine::processAudioBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!isEnabled)
    {
        buffer.clear(startSample, numSamples);
        return;
    }
    
    // Create audio block for processing
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    auto subBlock = audioBlock.getSubBlock(startSample, numSamples);
    
    // Measure input level
    float currentInputLevel = 0.0f;
    for (int channel = 0; channel < subBlock.getNumChannels(); ++channel)
    {
        auto channelData = subBlock.getChannelPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            currentInputLevel = juce::jmax(currentInputLevel, std::abs(channelData[sample]));
        }
    }
    inputLevelSmoothed.setTargetValue(currentInputLevel);
    inputLevel = inputLevelSmoothed.getNextValue();
    
    // Apply input gain
    for (int channel = 0; channel < subBlock.getNumChannels(); ++channel)
    {
        auto channelData = subBlock.getChannelPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] *= inputGain.getNextValue();
        }
    }
    
    // Process through compressor
    float compressionGR = compressor.processBlock(subBlock);
    
    // Process through limiter
    float limitingGR = limiter.processBlock(subBlock);
    
    // Combine gain reduction for metering
    currentGainReduction = juce::jmax(compressionGR, limitingGR);
    
    // Apply output gain
    for (int channel = 0; channel < subBlock.getNumChannels(); ++channel)
    {
        auto channelData = subBlock.getChannelPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] *= outputGain.getNextValue();
        }
    }
    
    // Measure output level
    float currentOutputLevel = 0.0f;
    for (int channel = 0; channel < subBlock.getNumChannels(); ++channel)
    {
        auto channelData = subBlock.getChannelPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            currentOutputLevel = juce::jmax(currentOutputLevel, std::abs(channelData[sample]));
        }
    }
    outputLevelSmoothed.setTargetValue(currentOutputLevel);
    outputLevel = outputLevelSmoothed.getNextValue();
    
    // Send processed audio to virtual device
    virtualDevice->processAudioBlock(buffer, startSample, numSamples);
}

void AudioEngine::releaseResources()
{
    compressor.releaseResources();
    limiter.releaseResources();
    
    if (virtualDevice)
        virtualDevice->releaseResources();
}

