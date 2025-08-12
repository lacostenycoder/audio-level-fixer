#pragma once

#include <JuceHeader.h>
#include "Compressor.h"
#include "Limiter.h"
#include "VirtualAudioDevice.h"

//==============================================================================
class AudioEngine
{
public:
    AudioEngine();
    ~AudioEngine();
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void processAudioBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void releaseResources();
    
    // Parameter setters
    void setEnabled(bool enabled) { isEnabled = enabled; }
    void setInputGain(float gainDb) { inputGain.setTargetValue(juce::Decibels::decibelsToGain(gainDb)); }
    void setOutputGain(float gainDb) { outputGain.setTargetValue(juce::Decibels::decibelsToGain(gainDb)); }
    
    void setCompressorThreshold(float thresholdDb) { compressor.setThreshold(thresholdDb); }
    void setCompressorRatio(float ratio) { compressor.setRatio(ratio); }
    void setCompressorAttack(float attackMs) { compressor.setAttack(attackMs); }
    void setCompressorRelease(float releaseMs) { compressor.setRelease(releaseMs); }
    
    void setLimiterCeiling(float ceilingDb) { limiter.setCeiling(ceilingDb); }
    void setLimiterLookahead(float lookaheadMs) { limiter.setLookahead(lookaheadMs); }
    
    // Getters for metering
    float getCurrentGainReduction() const { return currentGainReduction; }
    float getInputLevel() const { return inputLevel; }
    float getOutputLevel() const { return outputLevel; }
    
private:
    // Processing state
    bool isEnabled = false;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    
    // Gain controls
    juce::SmoothedValue<float> inputGain { 1.0f };
    juce::SmoothedValue<float> outputGain { 1.0f };
    
    // Processing components
    Compressor compressor;
    Limiter limiter;
    
    // Virtual audio device
    std::unique_ptr<VirtualAudioDevice> virtualDevice;
    
    // Metering
    float currentGainReduction = 0.0f;
    float inputLevel = 0.0f;
    float outputLevel = 0.0f;
    
    // Level smoothing for meters
    juce::SmoothedValue<float> inputLevelSmoothed { 0.0f };
    juce::SmoothedValue<float> outputLevelSmoothed { 0.0f };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};

