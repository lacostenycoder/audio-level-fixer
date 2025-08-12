#pragma once

#include <JuceHeader.h>

//==============================================================================
class Compressor
{
public:
    Compressor();
    ~Compressor();
    
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    float processBlock(juce::dsp::AudioBlock<float>& audioBlock);
    void releaseResources();
    
    // Parameter setters
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttack(float attackMs);
    void setRelease(float releaseMs);
    void setKnee(float kneeDb);
    void setMakeupGain(float gainDb);
    void setAutoMakeupGain(bool enabled);
    
    // Getters
    float getGainReduction() const { return currentGainReduction; }
    float getMakeupGain() const { return makeupGain; }
    
private:
    // Parameters
    float threshold = -20.0f;        // dB
    float ratio = 4.0f;              // ratio
    float attack = 1.0f;             // ms
    float release = 30.0f;           // ms
    float knee = 2.0f;               // dB
    float makeupGain = 0.0f;         // dB
    bool autoMakeupGain = true;
    
    // Processing state
    double sampleRate = 44100.0;
    int blockSize = 512;
    
    // Envelope follower
    float envelope = 0.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    
    // Gain reduction
    float currentGainReduction = 0.0f;
    juce::SmoothedValue<float> gainReductionSmoothed;
    
    // RMS detection
    static constexpr int rmsWindowSize = 64;
    juce::AudioBuffer<float> rmsBuffer;
    int rmsIndex = 0;
    
    // Helper functions
    void updateCoefficients();
    float calculateGainReduction(float inputLevel);
    float softKneeCompression(float inputLevel);
    float getRMSLevel(const juce::dsp::AudioBlock<float>& audioBlock);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};

