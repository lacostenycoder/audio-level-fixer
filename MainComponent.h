#pragma once

#include <JuceHeader.h>
#include "AudioEngine.h"

//==============================================================================
class MainComponent : public juce::AudioAppComponent,
                      public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

private:
    //==============================================================================
    // Audio processing
    std::unique_ptr<AudioEngine> audioEngine;
    
    // GUI Components
    juce::GroupComponent inputGroup, processingGroup, outputGroup;
    
    // Input controls
    juce::Slider inputGainSlider;
    juce::Label inputGainLabel;
    juce::ComboBox inputDeviceCombo;
    juce::Label inputDeviceLabel;
    
    // Processing controls
    juce::Slider compressorThresholdSlider, compressorRatioSlider;
    juce::Slider compressorAttackSlider, compressorReleaseSlider;
    juce::Label compressorThresholdLabel, compressorRatioLabel;
    juce::Label compressorAttackLabel, compressorReleaseLabel;
    
    juce::Slider limiterCeilingSlider, limiterLookaheadSlider;
    juce::Label limiterCeilingLabel, limiterLookaheadLabel;
    
    // Output controls
    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;
    juce::ComboBox outputDeviceCombo;
    juce::Label outputDeviceLabel;
    
    // Level meters
    juce::Rectangle<int> inputMeterBounds, outputMeterBounds, grMeterBounds;
    float inputLevel = 0.0f, outputLevel = 0.0f, gainReduction = 0.0f;
    
    // Control buttons
    juce::TextButton enableButton;
    juce::TextButton presetButton;
    
    // Preset management
    juce::ComboBox presetCombo;
    juce::Label presetLabel;
    
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
                     double min, double max, double defaultValue, const juce::String& suffix = "");
    void setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText);
    void updateDeviceLists();
    void loadPreset(int presetIndex);
    void saveCurrentAsPreset();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

