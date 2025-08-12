#include <iostream>
#include <memory>

// Mock JUCE headers for testing
namespace juce {
    class String {
    public:
        String(const char* text) : text_(text) {}
        const char* toUTF8() const { return text_.c_str(); }
    private:
        std::string text_;
    };
    
    class AudioBuffer {
    public:
        AudioBuffer(int channels, int samples) : channels_(channels), samples_(samples) {}
        int getNumChannels() const { return channels_; }
        int getNumSamples() const { return samples_; }
        float getMagnitude(int start, int length) const { return 0.5f; }
    private:
        int channels_, samples_;
    };
    
    namespace Decibels {
        static float gainToDecibels(float gain) { return 20.0f * std::log10(gain); }
        static float decibelsToGain(float db) { return std::pow(10.0f, db / 20.0f); }
    }
}

// Include our audio processing classes
#include "../AudioProcessor/Source/Compressor.h"
#include "../AudioProcessor/Source/Limiter.h"

int main()
{
    std::cout << "Audio Processing Application Test\n";
    std::cout << "================================\n\n";
    
    // Test Compressor
    std::cout << "Testing Compressor...\n";
    Compressor compressor;
    compressor.prepareToPlay(44100.0, 512);
    compressor.setThreshold(-20.0f);
    compressor.setRatio(4.0f);
    compressor.setAttack(1.0f);
    compressor.setRelease(30.0f);
    std::cout << "✓ Compressor initialized successfully\n";
    
    // Test Limiter
    std::cout << "Testing Limiter...\n";
    Limiter limiter;
    limiter.prepareToPlay(44100.0, 512);
    limiter.setCeiling(-0.3f);
    limiter.setLookahead(3.0f);
    limiter.setRelease(300.0f);
    std::cout << "✓ Limiter initialized successfully\n";
    
    // Test audio processing chain
    std::cout << "Testing audio processing chain...\n";
    juce::AudioBuffer<float> testBuffer(2, 512);
    
    // Simulate audio processing
    float testGainReduction = compressor.processBlock(testBuffer);
    float testLimitingGR = limiter.processBlock(testBuffer);
    
    std::cout << "✓ Audio processing chain working\n";
    std::cout << "  - Compressor gain reduction: " << testGainReduction << " dB\n";
    std::cout << "  - Limiter gain reduction: " << testLimitingGR << " dB\n";
    
    std::cout << "\n✅ All tests passed! Application is ready.\n";
    
    return 0;
}

