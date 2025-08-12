#pragma once

#include <JuceHeader.h>

//==============================================================================
class Limiter
{
public:
    Limiter();
    ~Limiter();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    float processBlock(juce::dsp::AudioBlock<float>& audioBlock);
    void releaseResources();
    
    // Parameter setters
    void setCeiling(float ceilingDb);
    void setLookahead(float lookaheadMs);
    void setRelease(float releaseMs);
    
    // Getters
    float getGainReduction() const { return currentGainReduction; }
    int getLatencySamples() const { return lookaheadSamples; }
    
private:
    // Parameters
    float ceiling = -0.3f;           // dB
    float lookahead = 3.0f;          // ms
    float releaseTime = 300.0f;      // ms
    
    // Processing state
    double sampleRate = 44100.0;
    int blockSize = 512;
    int lookaheadSamples = 0;
    
    // Delay line for lookahead
    juce::AudioBuffer<float> delayBuffer;
    int delayWriteIndex = 0;
    
    // Peak hold for moving minimum
    std::vector<float> peakHoldBuffer;
    int peakHoldSize = 0;
    int peakHoldIndex = 0;
    
    // Smoothing filter (cascaded box filters)
    static constexpr int numBoxFilters = 4;
    std::vector<std::vector<float>> boxFilterBuffers;
    std::vector<int> boxFilterIndices;
    std::vector<float> boxFilterSums;
    
    // Exponential release
    float releaseCoeff = 0.0f;
    float gainEnvelope = 1.0f;
    
    // Gain reduction tracking
    float currentGainReduction = 0.0f;
    
    // Helper functions
    void updateLookaheadSize();
    void updateReleaseCoeff();
    float calculateRequiredGain(float sampleValue);
    float applyPeakHold(float gainValue);
    float applySmoothingFilter(float gainValue);
    void processDelayLine(juce::dsp::AudioBlock<float>& audioBlock);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Limiter)
};

