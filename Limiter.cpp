#include "Limiter.h"

//==============================================================================
Limiter::Limiter()
{
    // Initialize box filter buffers
    boxFilterBuffers.resize(numBoxFilters);
    boxFilterIndices.resize(numBoxFilters, 0);
    boxFilterSums.resize(numBoxFilters, 0.0f);
}

Limiter::~Limiter()
{
    releaseResources();
}

void Limiter::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlock;
    
    updateLookaheadSize();
    updateReleaseCoeff();
    
    // Initialize delay buffer
    int delayBufferSize = lookaheadSamples + samplesPerBlock;
    delayBuffer.setSize(2, delayBufferSize);
    delayBuffer.clear();
    delayWriteIndex = 0;
    
    // Initialize peak hold buffer
    peakHoldSize = lookaheadSamples;
    peakHoldBuffer.resize(peakHoldSize, 1.0f);
    peakHoldIndex = 0;
    
    // Initialize box filter buffers
    int filterSize = lookaheadSamples / numBoxFilters;
    for (int i = 0; i < numBoxFilters; ++i)
    {
        boxFilterBuffers[i].resize(filterSize, 1.0f);
        boxFilterIndices[i] = 0;
        boxFilterSums[i] = static_cast<float>(filterSize);
    }
    
    gainEnvelope = 1.0f;
    currentGainReduction = 0.0f;
}

float Limiter::processBlock(juce::dsp::AudioBlock<float>& audioBlock)
{
    const int numChannels = static_cast<int>(audioBlock.getNumChannels());
    const int numSamples = static_cast<int>(audioBlock.getNumSamples());
    
    float maxGainReduction = 0.0f;
    
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Find peak across all channels for this sample
        float peakValue = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = audioBlock.getChannelPointer(channel);
            peakValue = juce::jmax(peakValue, std::abs(channelData[sample]));
        }
        
        // Calculate required gain to stay below ceiling
        float requiredGain = calculateRequiredGain(peakValue);
        
        // Apply peak hold (moving minimum)
        float peakHoldGain = applyPeakHold(requiredGain);
        
        // Apply smoothing filter
        float smoothedGain = applySmoothingFilter(peakHoldGain);
        
        // Apply exponential release
        if (smoothedGain < gainEnvelope)
        {
            // Immediate attack to required gain
            gainEnvelope = smoothedGain;
        }
        else
        {
            // Exponential release
            gainEnvelope += (smoothedGain - gainEnvelope) * (1.0f - releaseCoeff);
        }
        
        // Store delayed samples and apply gain to delayed audio
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = audioBlock.getChannelPointer(channel);
            
            // Store current sample in delay buffer
            delayBuffer.setSample(channel, delayWriteIndex, channelData[sample]);
            
            // Get delayed sample and apply limiting gain
            int readIndex = (delayWriteIndex - lookaheadSamples + delayBuffer.getNumSamples()) % delayBuffer.getNumSamples();
            float delayedSample = delayBuffer.getSample(channel, readIndex);
            channelData[sample] = delayedSample * gainEnvelope;
        }
        
        // Update delay write index
        delayWriteIndex = (delayWriteIndex + 1) % delayBuffer.getNumSamples();
        
        // Track maximum gain reduction for metering
        float gainReductionDb = juce::Decibels::gainToDecibels(gainEnvelope);
        maxGainReduction = juce::jmax(maxGainReduction, -gainReductionDb);
    }
    
    currentGainReduction = maxGainReduction;
    return currentGainReduction;
}

void Limiter::releaseResources()
{
    delayBuffer.clear();
    peakHoldBuffer.clear();
    
    for (auto& buffer : boxFilterBuffers)
        buffer.clear();
}

//==============================================================================
void Limiter::setCeiling(float ceilingDb)
{
    ceiling = ceilingDb;
}

void Limiter::setLookahead(float lookaheadMs)
{
    lookahead = juce::jmax(0.1f, lookaheadMs);
    updateLookaheadSize();
}

void Limiter::setRelease(float releaseMs)
{
    releaseTime = juce::jmax(1.0f, releaseMs);
    updateReleaseCoeff();
}

//==============================================================================
void Limiter::updateLookaheadSize()
{
    lookaheadSamples = static_cast<int>(lookahead * 0.001 * sampleRate);
    lookaheadSamples = juce::jmax(1, lookaheadSamples);
}

void Limiter::updateReleaseCoeff()
{
    // Calculate exponential release coefficient
    releaseCoeff = std::exp(-1.0f / (releaseTime * 0.001f * static_cast<float>(sampleRate)));
}

float Limiter::calculateRequiredGain(float sampleValue)
{
    if (sampleValue == 0.0f)
        return 1.0f;
    
    float ceilingLinear = juce::Decibels::decibelsToGain(ceiling);
    
    if (sampleValue <= ceilingLinear)
        return 1.0f;
    
    return ceilingLinear / sampleValue;
}

float Limiter::applyPeakHold(float gainValue)
{
    // Store current gain value in peak hold buffer
    peakHoldBuffer[peakHoldIndex] = gainValue;
    peakHoldIndex = (peakHoldIndex + 1) % peakHoldSize;
    
    // Find minimum gain in the peak hold window
    float minGain = 1.0f;
    for (float gain : peakHoldBuffer)
    {
        minGain = juce::jmin(minGain, gain);
    }
    
    return minGain;
}

float Limiter::applySmoothingFilter(float gainValue)
{
    float smoothedValue = gainValue;
    
    // Apply cascaded box filters for smooth gain curve
    for (int filterIndex = 0; filterIndex < numBoxFilters; ++filterIndex)
    {
        auto& buffer = boxFilterBuffers[filterIndex];
        auto& index = boxFilterIndices[filterIndex];
        auto& sum = boxFilterSums[filterIndex];
        
        if (buffer.empty())
            continue;
        
        // Remove old value from sum
        sum -= buffer[index];
        
        // Add new value
        buffer[index] = smoothedValue;
        sum += smoothedValue;
        
        // Calculate filtered output
        smoothedValue = sum / static_cast<float>(buffer.size());
        
        // Update index
        index = (index + 1) % static_cast<int>(buffer.size());
    }
    
    return smoothedValue;
}

