#include "Compressor.h"

//==============================================================================
Compressor::Compressor()
{
    gainReductionSmoothed.setCurrentAndTargetValue(0.0f);
}

Compressor::~Compressor()
{
    releaseResources();
}

void Compressor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlock;
    
    // Initialize RMS buffer
    rmsBuffer.setSize(2, rmsWindowSize);
    rmsBuffer.clear();
    rmsIndex = 0;
    
    // Setup smoothed values
    gainReductionSmoothed.reset(sampleRate, 0.01); // 10ms smoothing
    
    // Update coefficients
    updateCoefficients();
}

float Compressor::processBlock(juce::dsp::AudioBlock<float>& audioBlock)
{
    const int numChannels = static_cast<int>(audioBlock.getNumChannels());
    const int numSamples = static_cast<int>(audioBlock.getNumSamples());
    
    // Get RMS level of input
    float rmsLevel = getRMSLevel(audioBlock);
    
    // Convert to dB
    float rmsLevelDb = rmsLevel > 0.0f ? juce::Decibels::gainToDecibels(rmsLevel) : -100.0f;
    
    // Calculate required gain reduction
    float targetGainReduction = calculateGainReduction(rmsLevelDb);
    
    // Apply envelope follower
    if (targetGainReduction > envelope)
    {
        // Attack phase
        envelope = targetGainReduction + (envelope - targetGainReduction) * attackCoeff;
    }
    else
    {
        // Release phase
        envelope = targetGainReduction + (envelope - targetGainReduction) * releaseCoeff;
    }
    
    // Smooth the gain reduction for audio application
    gainReductionSmoothed.setTargetValue(envelope);
    
    // Apply gain reduction to audio
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float gainReduction = gainReductionSmoothed.getNextValue();
        float gain = juce::Decibels::decibelsToGain(-gainReduction + makeupGain);
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = audioBlock.getChannelPointer(channel);
            channelData[sample] *= gain;
        }
    }
    
    currentGainReduction = envelope;
    return currentGainReduction;
}

void Compressor::releaseResources()
{
    rmsBuffer.clear();
}

//==============================================================================
void Compressor::setThreshold(float thresholdDb)
{
    threshold = thresholdDb;
    if (autoMakeupGain)
    {
        // Calculate automatic makeup gain
        float compressionAmount = (threshold - (-60.0f)) / ratio;
        makeupGain = compressionAmount * 0.5f; // Conservative makeup gain
    }
}

void Compressor::setRatio(float ratio)
{
    this->ratio = juce::jmax(1.0f, ratio);
    if (autoMakeupGain)
    {
        float compressionAmount = (threshold - (-60.0f)) / this->ratio;
        makeupGain = compressionAmount * 0.5f;
    }
}

void Compressor::setAttack(float attackMs)
{
    attack = juce::jmax(0.1f, attackMs);
    updateCoefficients();
}

void Compressor::setRelease(float releaseMs)
{
    release = juce::jmax(1.0f, releaseMs);
    updateCoefficients();
}

void Compressor::setKnee(float kneeDb)
{
    knee = juce::jmax(0.0f, kneeDb);
}

void Compressor::setMakeupGain(float gainDb)
{
    makeupGain = gainDb;
    autoMakeupGain = false;
}

void Compressor::setAutoMakeupGain(bool enabled)
{
    autoMakeupGain = enabled;
    if (enabled)
    {
        float compressionAmount = (threshold - (-60.0f)) / ratio;
        makeupGain = compressionAmount * 0.5f;
    }
}

//==============================================================================
void Compressor::updateCoefficients()
{
    // Calculate attack and release coefficients
    // Using exponential approach for smooth envelope following
    attackCoeff = std::exp(-1.0f / (attack * 0.001f * static_cast<float>(sampleRate)));
    releaseCoeff = std::exp(-1.0f / (release * 0.001f * static_cast<float>(sampleRate)));
}

float Compressor::calculateGainReduction(float inputLevelDb)
{
    if (inputLevelDb <= threshold)
        return 0.0f;
    
    if (knee > 0.0f)
    {
        return softKneeCompression(inputLevelDb);
    }
    else
    {
        // Hard knee compression
        float overThreshold = inputLevelDb - threshold;
        return overThreshold - (overThreshold / ratio);
    }
}

float Compressor::softKneeCompression(float inputLevelDb)
{
    float kneeStart = threshold - (knee / 2.0f);
    float kneeEnd = threshold + (knee / 2.0f);
    
    if (inputLevelDb <= kneeStart)
    {
        return 0.0f;
    }
    else if (inputLevelDb >= kneeEnd)
    {
        // Above knee - full compression
        float overThreshold = inputLevelDb - threshold;
        return overThreshold - (overThreshold / ratio);
    }
    else
    {
        // In knee region - gradual compression
        float kneeRatio = (inputLevelDb - kneeStart) / knee;
        float currentRatio = 1.0f + (ratio - 1.0f) * kneeRatio;
        float overThreshold = inputLevelDb - threshold;
        return overThreshold - (overThreshold / currentRatio);
    }
}

float Compressor::getRMSLevel(const juce::dsp::AudioBlock<float>& audioBlock)
{
    const int numChannels = static_cast<int>(audioBlock.getNumChannels());
    const int numSamples = static_cast<int>(audioBlock.getNumSamples());
    
    float sumSquares = 0.0f;
    int sampleCount = 0;
    
    // Calculate RMS over the current block
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = audioBlock.getChannelPointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float sampleValue = channelData[sample];
            sumSquares += sampleValue * sampleValue;
            sampleCount++;
        }
    }
    
    if (sampleCount > 0)
    {
        return std::sqrt(sumSquares / static_cast<float>(sampleCount));
    }
    
    return 0.0f;
}

