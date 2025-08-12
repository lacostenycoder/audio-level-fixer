#pragma once
#include <juce_audio_devices/juce_audio_devices.h>
#include <atomic>

// Minimal realtime engine: input -> noise gate -> gentle comp/limiter -> output.
// Exposes peak meters and gain reduction for UI.
class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    // Parameters (hook up UI later if desired)
    std::atomic<float> inputGain { 1.0f };     // linear
    std::atomic<float> outputGain{ 1.0f };     // linear

    // Noise Gate parameters
    std::atomic<float> gateThreshold { -60.0f }; // dB
    std::atomic<float> gateRatio { 10.0f };     // ratio
    std::atomic<float> gateAttack { 1.0f };     // ms
    std::atomic<float> gateRelease { 100.0f };  // ms

    // Compressor parameters
    std::atomic<float> threshDb  { -18.0f };
    std::atomic<float> ratio     { 3.0f };
    std::atomic<float> ceilingDb { -1.0f };

    // Meter taps (read by UI timer)
    std::atomic<float> inPeak  { 0.0f }; // 0..1
    std::atomic<float> outPeak { 0.0f }; // 0..1
    std::atomic<float> grDb    { 0.0f }; // positive reduction amount in dB

    void audioDeviceAboutToStart(juce::AudioIODevice* dev) override
    {
        fs = dev ? dev->getCurrentSampleRate() : 48000.0;
        env = 0.0f;
        gateEnv = 0.0f;
        inPeak.store(0); outPeak.store(0); grDb.store(0);
    }
    void audioDeviceStopped() override {}

    // JUCE 8 uses this method - implement the audio processing here
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const juce::AudioIODeviceCallbackContext& context) override;

private:
    double fs = 48000.0;
    float env = 0.0f; // simple peak follower
    float gateEnv = 0.0f; // noise gate envelope follower
};
