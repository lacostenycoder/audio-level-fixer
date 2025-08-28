#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include "AudioEngine.h"
#include "Compressor.h"
#include "Limiter.h"

// Custom audio meter with green/yellow/red colors
class AudioMeter : public juce::Component
{
public:
    AudioMeter(const juce::String& name);
    void setValue(float newValue);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::String meterName;
    float currentValue = 0.0f;
    float peakValue = 0.0f;
    double lastUpdateTime = 0.0;
    float peakHoldTime = 1.0f; // seconds
    float peakDecayPerSecond = 0.5f; // units per second

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMeter)
};

class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Device controls
    juce::ComboBox inputDeviceBox, outputDeviceBox;
    juce::TextButton enableButton { "Enable Processing" };

    // Audio processing controls - all vertical sliders
    juce::Slider inputGainSlider, outputGainSlider;

    // Noise Gate controls
    juce::Slider gateThresholdSlider, gateRatioSlider, gateAttackSlider, gateReleaseSlider;

    // Compressor controls
    juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider;

    // Limiter controls
    juce::Slider ceilingSlider, lookaheadSlider;

    // Additional controls
    juce::Slider kneeSlider, makeupGainSlider;

    // Labels for sliders
    juce::Label inputGainLabel, outputGainLabel;
    juce::Label gateThresholdLabel, gateRatioLabel, gateAttackLabel, gateReleaseLabel;
    juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel;
    juce::Label ceilingLabel, lookaheadLabel;
    juce::Label kneeLabel, makeupGainLabel;

    // Preset management
    juce::ComboBox presetBox;
    juce::TextButton savePresetButton { "Save Preset" };
    juce::TextButton loadPresetButton { "Load Preset" };
    juce::TextButton deletePresetButton { "Delete Preset" };

    // Custom meters
    std::unique_ptr<AudioMeter> inputMeter;
    std::unique_ptr<AudioMeter> outputMeter;
    std::unique_ptr<AudioMeter> gainReductionMeter;

    // Values you can pipe into your on-screen meter components
    float lastIn = 0.f, lastOut = 0.f, lastGR = 0.f;

    juce::AudioDeviceManager deviceManager;
    AudioEngine engine;
    Compressor compressor;
    Limiter limiter;
    bool processingOn = false;

    void refreshDeviceLists();
    void setInputDevice(const juce::String& name);
    void setOutputDevice(const juce::String& name);
    void toggleProcessing();

    void setupSliders();
    void setupLabels();
    void setupPresets();
    void updateEngineParameters();
    void loadPreset(const juce::String& presetName);
    void savePreset(const juce::String& presetName);
    void refreshPresetList();
    
    // Helper methods for file-based preset management
    bool loadPresetFromFile(const juce::String& presetName);
    bool loadPresetFromFile(const juce::String& presetName, const juce::File& presetFile);
    juce::File getPresetFile(const juce::String& presetName);
    juce::File getPresetDirectory();
    bool isBuiltInPreset(const juce::String& presetName);
    juce::String cleanPresetName(const juce::String& name);
    void doSavePreset(const juce::String& presetName, const juce::File& presetFile);
    
    // Helper methods for lambda callbacks
    void handleSavePresetClick();
    void handleLoadPresetClick();
    void handleDeletePresetClick();
    
    // Custom LookAndFeel for larger fonts
    class CustomLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        juce::Font getComboBoxFont(juce::ComboBox&) override
        {
            return juce::Font(16.0f);
        }
        
        juce::Font getTextButtonFont(juce::TextButton&, int) override
        {
            return juce::Font(16.0f);
        }
    };
    
    CustomLookAndFeel customLookAndFeel;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
