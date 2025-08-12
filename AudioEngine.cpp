#include "AudioEngine.h"
#include <juce_dsp/juce_dsp.h>

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* in,
                                                    int numIn,
                                                    float* const* out,
                                                    int numOut,
                                                    int numSamples,
                                                    const juce::AudioIODeviceCallbackContext&)
{
    if (numOut == 0) return;

    for (int ch = 0; ch < numOut; ++ch)
        juce::FloatVectorOperations::clear(out[ch], numSamples);

    if (numIn == 0) return;

    const float inG   = inputGain.load();
    const float outG  = outputGain.load();

    // Noise Gate parameters
    const float gateThr = juce::Decibels::decibelsToGain(gateThreshold.load());
    const float gateR   = juce::jmax(1.0f, gateRatio.load());
    const float gateA   = gateAttack.load() / 1000.0f;  // convert ms to seconds
    const float gateRel = gateRelease.load() / 1000.0f;  // convert ms to seconds

    // Compressor parameters
    const float thr   = juce::Decibels::decibelsToGain(threshDb.load());
    const float ceilG = juce::Decibels::decibelsToGain(ceilingDb.load());
    const float R     = juce::jmax(1.0f, ratio.load());

    float pkIn = 0.0f, pkOut = 0.0f, maxGr = 0.0f;

    for (int n = 0; n < numSamples; ++n)
    {
        float x = in[0][n] * inG;           // mono from first input channel
        pkIn = juce::jmax(pkIn, std::abs(x));

        // Noise Gate processing
        const float gateLevel = std::abs(x);
        gateEnv = 0.99f * gateEnv + 0.01f * gateLevel;

        float gateGain = 1.0f;
        if (gateEnv < gateThr) {
            const float under = gateThr / gateEnv;
            const float underDb = juce::Decibels::gainToDecibels(under);
            const float gateRedDb = underDb - (underDb / gateR);
            gateGain = juce::Decibels::decibelsToGain(-gateRedDb);
        }

        x *= gateGain;

        // detector
        const float level = std::abs(x);
        env = 0.99f * env + 0.01f * level;

        float gain = 1.0f;
        if (env > thr) {
            const float over   = env / thr;
            const float overDb = juce::Decibels::gainToDecibels(over);
            const float redDb  = overDb - (overDb / R);   // reduction amount
            gain = juce::Decibels::decibelsToGain(-redDb);
            maxGr = juce::jmax(maxGr, redDb);
        }

        float y = x * gain;
        y = juce::jlimit(-ceilG, ceilG, y);
        y *= outG;

        for (int ch = 0; ch < numOut; ++ch)
            out[ch][n] = y;

        pkOut = juce::jmax(pkOut, std::abs(y));
    }

    inPeak.store(pkIn);
    outPeak.store(pkOut);
    grDb.store(maxGr);
}
