#include "MainComponent.h"

// AudioMeter implementation
AudioMeter::AudioMeter(const juce::String& name) : meterName(name)
{
    lastUpdateTime = juce::Time::getMillisecondCounterHiRes();
}

void AudioMeter::setValue(float newValue)
{
    currentValue = newValue;

    double now = juce::Time::getMillisecondCounterHiRes();
    double elapsedSeconds = (now - lastUpdateTime) * 0.001;
    lastUpdateTime = now;

    if (newValue > peakValue)
    {
        peakValue = newValue;
        peakHoldTime = 1.0f; // Reset hold time
    }
    else
    {
        // Decrease hold time
        peakHoldTime -= elapsedSeconds;
        if (peakHoldTime < 0)
            peakHoldTime = 0;
    }

    // Decay peak value if hold time has expired
    if (peakHoldTime <= 0)
    {
        // Decay peakValue at a rate of peakDecayPerSecond per second
        peakValue -= peakDecayPerSecond * elapsedSeconds;
        if (peakValue < currentValue)
            peakValue = currentValue;
    }

    repaint();
}

void AudioMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour(juce::Colours::black);
    g.fillRect(bounds);

    // Border
    g.setColour(juce::Colours::white);
    g.drawRect(bounds, 1.0f);

    // Calculate meter height based on value
    float meterHeight = bounds.getHeight() * currentValue;
    float peakHeight = bounds.getHeight() * peakValue;

    // Draw meter segments with proper colors
    auto meterBounds = bounds.reduced(2.0f);

    // Green zone (0 to -20 dB)
    g.setColour(juce::Colours::green);
    float greenHeight = meterBounds.getHeight() * 0.63f; // -20 dB = 0.1 in linear
    if (meterHeight > 0)
    {
        float greenMeterHeight = juce::jmin(meterHeight, greenHeight);
        g.fillRect(meterBounds.removeFromBottom(greenMeterHeight));
    }

    // Yellow zone (-20 to -6 dB)
    if (meterHeight > greenHeight)
    {
        g.setColour(juce::Colours::yellow);
        float yellowHeight = meterBounds.getHeight() * 0.25f; // -6 dB = 0.5 in linear
        float yellowMeterHeight = juce::jmin(meterHeight - greenHeight, yellowHeight);
        if (yellowMeterHeight > 0)
        {
            g.fillRect(meterBounds.removeFromBottom(yellowMeterHeight));
        }
    }

    // Red zone (-6 to 0 dB)
    if (meterHeight > greenHeight + (bounds.getHeight() * 0.25f))
    {
        g.setColour(juce::Colours::red);
        float redMeterHeight = meterHeight - greenHeight - (bounds.getHeight() * 0.25f);
        if (redMeterHeight > 0)
        {
            g.fillRect(meterBounds.removeFromBottom(redMeterHeight));
        }
    }

    // Draw peak indicator
    if (peakValue > 0)
    {
        g.setColour(juce::Colours::white);
        float peakY = bounds.getBottom() - (bounds.getHeight() * peakValue);
        g.drawLine(bounds.getX(), peakY, bounds.getRight(), peakY, 2.0f);
    }

    // Draw label
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText(meterName, bounds.removeFromTop(20), juce::Justification::centred);
}

void AudioMeter::resized()
{
    // No special resizing needed
}

MainComponent::MainComponent()
{
    // 1 input channel, 2 output channels (JUCE will adapt to device layout)
    deviceManager.initialise (1, 2, nullptr, true);

    // Create custom meters
    inputMeter = std::make_unique<AudioMeter>("Input");
    outputMeter = std::make_unique<AudioMeter>("Output");
    gainReductionMeter = std::make_unique<AudioMeter>("Gain Red");

    // Add all UI components
    addAndMakeVisible(inputDeviceBox);
    addAndMakeVisible(outputDeviceBox);
    addAndMakeVisible(enableButton);

    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);

    addAndMakeVisible(gateThresholdSlider);
    addAndMakeVisible(gateRatioSlider);
    addAndMakeVisible(gateAttackSlider);
    addAndMakeVisible(gateReleaseSlider);

    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(ceilingSlider);
    addAndMakeVisible(lookaheadSlider);
    addAndMakeVisible(kneeSlider);
    addAndMakeVisible(makeupGainSlider);

    addAndMakeVisible(inputGainLabel);
    addAndMakeVisible(outputGainLabel);
    addAndMakeVisible(gateThresholdLabel);
    addAndMakeVisible(gateRatioLabel);
    addAndMakeVisible(gateAttackLabel);
    addAndMakeVisible(gateReleaseLabel);
    addAndMakeVisible(thresholdLabel);
    addAndMakeVisible(ratioLabel);
    addAndMakeVisible(attackLabel);
    addAndMakeVisible(releaseLabel);
    addAndMakeVisible(ceilingLabel);
    addAndMakeVisible(lookaheadLabel);
    addAndMakeVisible(kneeLabel);
    addAndMakeVisible(makeupGainLabel);

    addAndMakeVisible(presetBox);
    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(loadPresetButton);
    addAndMakeVisible(deletePresetButton);

    addAndMakeVisible(inputMeter.get());
    addAndMakeVisible(outputMeter.get());
    addAndMakeVisible(gainReductionMeter.get());

    // Setup sliders and labels
    setupSliders();
    setupLabels();
    setupPresets();

    // Apply custom look and feel for larger fonts to all UI components
    inputDeviceBox.setLookAndFeel(&customLookAndFeel);
    outputDeviceBox.setLookAndFeel(&customLookAndFeel);
    presetBox.setLookAndFeel(&customLookAndFeel);
    enableButton.setLookAndFeel(&customLookAndFeel);
    savePresetButton.setLookAndFeel(&customLookAndFeel);
    loadPresetButton.setLookAndFeel(&customLookAndFeel);
    deletePresetButton.setLookAndFeel(&customLookAndFeel);

    // Setup callbacks
    inputDeviceBox.onChange  = [this]{ setInputDevice (inputDeviceBox.getText());  };
    outputDeviceBox.onChange = [this]{ setOutputDevice(outputDeviceBox.getText()); };
    enableButton.onClick     = [this]{ toggleProcessing(); };

    savePresetButton.onClick = [this]{ handleSavePresetClick(); };
    loadPresetButton.onClick = [this]{ handleLoadPresetClick(); };
    deletePresetButton.onClick = [this]{ handleDeletePresetClick(); };
    presetBox.onChange = [this]{ loadPreset(presetBox.getText()); };

    refreshDeviceLists();
    startTimerHz(30); // meter refresh
}

MainComponent::~MainComponent()
{
    if (processingOn)
        deviceManager.removeAudioCallback(&engine);
    
    // Reset look and feel to prevent dangling pointers
    inputDeviceBox.setLookAndFeel(nullptr);
    outputDeviceBox.setLookAndFeel(nullptr);
    presetBox.setLookAndFeel(nullptr);
    enableButton.setLookAndFeel(nullptr);
    savePresetButton.setLookAndFeel(nullptr);
    loadPresetButton.setLookAndFeel(nullptr);
    deletePresetButton.setLookAndFeel(nullptr);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Draw section headers
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawText("Audio Processor", getLocalBounds().removeFromTop(40), juce::Justification::centred);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    // Calculate optimal heights based on DPI scaling with larger minimums
    const int comboBoxHeight = juce::jmax(30, static_cast<int>(30 * getDesktopScaleFactor()));
    const int buttonHeight = juce::jmax(35, static_cast<int>(35 * getDesktopScaleFactor()));
    const int sliderHeight = juce::jmax(80, static_cast<int>(80 * getDesktopScaleFactor()));
    const int labelHeight = juce::jmax(20, static_cast<int>(20 * getDesktopScaleFactor()));
    
    // Title area
    auto titleArea = bounds.removeFromTop(40);

    // Device controls area - dynamic height with more padding
    const int deviceAreaHeight = comboBoxHeight + buttonHeight + 30; // more padding
    auto deviceArea = bounds.removeFromTop(deviceAreaHeight);
    inputDeviceBox.setBounds(deviceArea.removeFromLeft(deviceArea.getWidth() / 2 - 5).removeFromTop(comboBoxHeight));
    outputDeviceBox.setBounds(deviceArea.removeFromRight(deviceArea.getWidth() / 2 - 5).removeFromTop(comboBoxHeight));
    deviceArea.removeFromTop(10); // add space before button
    enableButton.setBounds(deviceArea.removeFromTop(buttonHeight));

    // Meters area (right side)
    auto metersArea = bounds.removeFromRight(80);
    const int meterHeight = juce::jmax(120, static_cast<int>(120 * getDesktopScaleFactor()));
    inputMeter->setBounds(metersArea.removeFromTop(meterHeight));
    metersArea.removeFromTop(10);
    outputMeter->setBounds(metersArea.removeFromTop(meterHeight));
    metersArea.removeFromTop(10);
    gainReductionMeter->setBounds(metersArea.removeFromTop(meterHeight));

    // Preset area - dynamic height for 3 buttons
    const int presetAreaHeight = comboBoxHeight + (buttonHeight * 3) + 15; // padding between buttons
    auto presetArea = bounds.removeFromTop(presetAreaHeight);
    presetBox.setBounds(presetArea.removeFromLeft(presetArea.getWidth() / 2 - 5).removeFromTop(comboBoxHeight));
    auto buttonArea = presetArea.removeFromRight(presetArea.getWidth() / 2 - 5);
    savePresetButton.setBounds(buttonArea.removeFromTop(buttonHeight));
    buttonArea.removeFromTop(5);
    loadPresetButton.setBounds(buttonArea.removeFromTop(buttonHeight));
    buttonArea.removeFromTop(5);
    deletePresetButton.setBounds(buttonArea.removeFromTop(buttonHeight));

    // Controls area (left side) - organized in columns with better spacing
    auto controlsArea = bounds;
    
    // Calculate available height and adjust slider height if needed
    int availableHeight = controlsArea.getHeight();
    int neededHeight = 4 * (labelHeight + sliderHeight) + 30; // 30px for spacing
    
    // Adjust slider height if we don't have enough space
    int adjustedSliderHeight = sliderHeight;
    if (neededHeight > availableHeight)
    {
        adjustedSliderHeight = juce::jmax(60, (availableHeight - 4 * labelHeight - 30) / 4);
    }

    // Column 1: Input/Output gains
    auto col1 = controlsArea.removeFromLeft(controlsArea.getWidth() / 4);
    inputGainLabel.setBounds(col1.removeFromTop(labelHeight));
    inputGainSlider.setBounds(col1.removeFromTop(adjustedSliderHeight));
    col1.removeFromTop(10);
    outputGainLabel.setBounds(col1.removeFromTop(labelHeight));
    outputGainSlider.setBounds(col1.removeFromTop(adjustedSliderHeight));

    // Column 2: Noise Gate
    auto col2 = controlsArea.removeFromLeft(controlsArea.getWidth() / 3);
    gateThresholdLabel.setBounds(col2.removeFromTop(labelHeight));
    gateThresholdSlider.setBounds(col2.removeFromTop(adjustedSliderHeight));
    col2.removeFromTop(10);
    gateRatioLabel.setBounds(col2.removeFromTop(labelHeight));
    gateRatioSlider.setBounds(col2.removeFromTop(adjustedSliderHeight));
    col2.removeFromTop(10);
    gateAttackLabel.setBounds(col2.removeFromTop(labelHeight));
    gateAttackSlider.setBounds(col2.removeFromTop(adjustedSliderHeight));
    col2.removeFromTop(10);
    gateReleaseLabel.setBounds(col2.removeFromTop(labelHeight));
    gateReleaseSlider.setBounds(col2.removeFromTop(adjustedSliderHeight));

    // Column 3: Compressor
    auto col3 = controlsArea.removeFromLeft(controlsArea.getWidth() / 2);
    thresholdLabel.setBounds(col3.removeFromTop(labelHeight));
    thresholdSlider.setBounds(col3.removeFromTop(adjustedSliderHeight));
    col3.removeFromTop(10);
    ratioLabel.setBounds(col3.removeFromTop(labelHeight));
    ratioSlider.setBounds(col3.removeFromTop(adjustedSliderHeight));
    col3.removeFromTop(10);
    attackLabel.setBounds(col3.removeFromTop(labelHeight));
    attackSlider.setBounds(col3.removeFromTop(adjustedSliderHeight));
    col3.removeFromTop(10);
    releaseLabel.setBounds(col3.removeFromTop(labelHeight));
    releaseSlider.setBounds(col3.removeFromTop(adjustedSliderHeight));

    // Column 4: Limiter and additional controls - ensure all controls fit
    auto col4 = controlsArea;
    ceilingLabel.setBounds(col4.removeFromTop(labelHeight));
    ceilingSlider.setBounds(col4.removeFromTop(adjustedSliderHeight));
    col4.removeFromTop(10);
    lookaheadLabel.setBounds(col4.removeFromTop(labelHeight));
    lookaheadSlider.setBounds(col4.removeFromTop(adjustedSliderHeight));
    col4.removeFromTop(10);
    kneeLabel.setBounds(col4.removeFromTop(labelHeight));
    kneeSlider.setBounds(col4.removeFromTop(adjustedSliderHeight));
    col4.removeFromTop(10);
    makeupGainLabel.setBounds(col4.removeFromTop(labelHeight));
    makeupGainSlider.setBounds(col4.removeFromTop(adjustedSliderHeight));
}

void MainComponent::setupSliders()
{
    // All sliders are now vertical
    auto setupVerticalSlider = [](juce::Slider& slider, double min, double max, double value,
                                  const std::function<void()>& callback) {
        slider.setRange(min, max, (max - min) / 1000.0);
        slider.setValue(value);
        slider.setSliderStyle(juce::Slider::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        slider.onValueChange = callback;
    };

    // Input/Output gains
    setupVerticalSlider(inputGainSlider, 0.0, 10.0, 1.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(outputGainSlider, 0.0, 10.0, 1.0, [this]{ updateEngineParameters(); });

    // Noise Gate controls
    setupVerticalSlider(gateThresholdSlider, -80.0, 0.0, -60.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(gateRatioSlider, 1.0, 50.0, 10.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(gateAttackSlider, 0.1, 100.0, 1.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(gateReleaseSlider, 10.0, 1000.0, 100.0, [this]{ updateEngineParameters(); });

    // Compressor controls
    setupVerticalSlider(thresholdSlider, -60.0, 0.0, -18.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(ratioSlider, 1.0, 20.0, 3.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(attackSlider, 0.1, 100.0, 1.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(releaseSlider, 10.0, 1000.0, 30.0, [this]{ updateEngineParameters(); });

    // Limiter controls
    setupVerticalSlider(ceilingSlider, -20.0, 0.0, -1.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(lookaheadSlider, 0.0, 10.0, 3.0, [this]{ updateEngineParameters(); });

    // Additional controls
    setupVerticalSlider(kneeSlider, 0.0, 20.0, 2.0, [this]{ updateEngineParameters(); });
    setupVerticalSlider(makeupGainSlider, -20.0, 20.0, 0.0, [this]{ updateEngineParameters(); });
}

void MainComponent::setupLabels()
{
    inputGainLabel.setText("Input Gain", juce::dontSendNotification);
    inputGainLabel.setJustificationType(juce::Justification::centred);

    outputGainLabel.setText("Output Gain", juce::dontSendNotification);
    outputGainLabel.setJustificationType(juce::Justification::centred);

    gateThresholdLabel.setText("Gate Thresh", juce::dontSendNotification);
    gateThresholdLabel.setJustificationType(juce::Justification::centred);

    gateRatioLabel.setText("Gate Ratio", juce::dontSendNotification);
    gateRatioLabel.setJustificationType(juce::Justification::centred);

    gateAttackLabel.setText("Gate Attack", juce::dontSendNotification);
    gateAttackLabel.setJustificationType(juce::Justification::centred);

    gateReleaseLabel.setText("Gate Release", juce::dontSendNotification);
    gateReleaseLabel.setJustificationType(juce::Justification::centred);

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);

    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.setJustificationType(juce::Justification::centred);

    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);

    ceilingLabel.setText("Ceiling", juce::dontSendNotification);
    ceilingLabel.setJustificationType(juce::Justification::centred);

    lookaheadLabel.setText("Lookahead", juce::dontSendNotification);
    lookaheadLabel.setJustificationType(juce::Justification::centred);

    kneeLabel.setText("Knee", juce::dontSendNotification);
    kneeLabel.setJustificationType(juce::Justification::centred);

    makeupGainLabel.setText("Makeup", juce::dontSendNotification);
    makeupGainLabel.setJustificationType(juce::Justification::centred);
}

void MainComponent::setupPresets()
{
    refreshPresetList();
}

void MainComponent::updateEngineParameters()
{
    // Update AudioEngine parameters
    engine.inputGain.store(inputGainSlider.getValue());
    engine.outputGain.store(outputGainSlider.getValue());

    // Update Noise Gate parameters
    engine.gateThreshold.store(gateThresholdSlider.getValue());
    engine.gateRatio.store(gateRatioSlider.getValue());
    engine.gateAttack.store(gateAttackSlider.getValue());
    engine.gateRelease.store(gateReleaseSlider.getValue());

    // Update Compressor parameters
    engine.threshDb.store(thresholdSlider.getValue());
    engine.ratio.store(ratioSlider.getValue());
    engine.ceilingDb.store(ceilingSlider.getValue());

    // Update Compressor and Limiter parameters
    compressor.setThreshold(thresholdSlider.getValue());
    compressor.setRatio(ratioSlider.getValue());
    compressor.setAttack(attackSlider.getValue());
    compressor.setRelease(releaseSlider.getValue());
    compressor.setKnee(kneeSlider.getValue());
    compressor.setMakeupGain(makeupGainSlider.getValue());

    limiter.setCeiling(ceilingSlider.getValue());
    limiter.setLookahead(lookaheadSlider.getValue());
    limiter.setRelease(releaseSlider.getValue());
}

void MainComponent::loadPreset(const juce::String& presetName)
{
    // First try to load from file, if that fails use hardcoded values
    if (loadPresetFromFile(presetName))
    {
        updateEngineParameters();
        return;
    }
    
    // Fallback to hardcoded presets
    if (presetName == "Default")
    {
        inputGainSlider.setValue(1.0);
        outputGainSlider.setValue(1.0);
        gateThresholdSlider.setValue(-60.0);
        gateRatioSlider.setValue(10.0);
        gateAttackSlider.setValue(1.0);
        gateReleaseSlider.setValue(100.0);
        thresholdSlider.setValue(-18.0);
        ratioSlider.setValue(3.0);
        attackSlider.setValue(1.0);
        releaseSlider.setValue(30.0);
        ceilingSlider.setValue(-1.0);
        lookaheadSlider.setValue(3.0);
        kneeSlider.setValue(2.0);
        makeupGainSlider.setValue(0.0);
    }
    else if (presetName == "Podcast")
    {
        inputGainSlider.setValue(1.2);
        outputGainSlider.setValue(0.8);
        gateThresholdSlider.setValue(-50.0);
        gateRatioSlider.setValue(15.0);
        gateAttackSlider.setValue(2.0);
        gateReleaseSlider.setValue(150.0);
        thresholdSlider.setValue(-24.0);
        ratioSlider.setValue(4.0);
        attackSlider.setValue(2.0);
        releaseSlider.setValue(50.0);
        ceilingSlider.setValue(-2.0);
        lookaheadSlider.setValue(2.0);
        kneeSlider.setValue(3.0);
        makeupGainSlider.setValue(2.0);
    }
    else if (presetName == "Streaming")
    {
        inputGainSlider.setValue(1.5);
        outputGainSlider.setValue(0.7);
        gateThresholdSlider.setValue(-45.0);
        gateRatioSlider.setValue(20.0);
        gateAttackSlider.setValue(1.5);
        gateReleaseSlider.setValue(120.0);
        thresholdSlider.setValue(-20.0);
        ratioSlider.setValue(6.0);
        attackSlider.setValue(1.5);
        releaseSlider.setValue(40.0);
        ceilingSlider.setValue(-1.5);
        lookaheadSlider.setValue(4.0);
        kneeSlider.setValue(2.5);
        makeupGainSlider.setValue(3.0);
    }
    else if (presetName == "VoiceOver")
    {
        inputGainSlider.setValue(1.8);
        outputGainSlider.setValue(0.6);
        gateThresholdSlider.setValue(-40.0);
        gateRatioSlider.setValue(25.0);
        gateAttackSlider.setValue(0.5);
        gateReleaseSlider.setValue(80.0);
        thresholdSlider.setValue(-16.0);
        ratioSlider.setValue(8.0);
        attackSlider.setValue(0.5);
        releaseSlider.setValue(25.0);
        ceilingSlider.setValue(-0.5);
        lookaheadSlider.setValue(5.0);
        kneeSlider.setValue(1.5);
        makeupGainSlider.setValue(4.0);
    }
    else if (presetName == "SlammedUp")
    {
        inputGainSlider.setValue(10.0);  // Input Gain=10.0
        outputGainSlider.setValue(1.0);  // Output Gain=0.0 but using 1.0 as base
        gateThresholdSlider.setValue(-44.0);  // NoiseGate Threshold=-40.0
        gateRatioSlider.setValue(21.0);  // NoiseGate Ratio=21.0
        gateAttackSlider.setValue(0.1);  // NoiseGate Attack=0.1
        gateReleaseSlider.setValue(400.0);  // NoiseGate Release=400.0
        thresholdSlider.setValue(-18.0);  // Compressor Threshold=-18.0
        ratioSlider.setValue(3.6);  // Compressor Ratio=3.0
        attackSlider.setValue(0.2);  // Compressor Attack=0.5
        releaseSlider.setValue(175.0);  // Compressor Release=20.0
        ceilingSlider.setValue(-1.5);  // Limiter Ceiling=-1.66
        lookaheadSlider.setValue(5.0);  // Limiter Lookahead=5.0
        kneeSlider.setValue(1.5);  // Compressor Knee=1.5
        makeupGainSlider.setValue(4.0);  // MakeupGain=4.0
    }

    updateEngineParameters();
}

void MainComponent::savePreset(const juce::String& presetName)
{
    if (presetName.isEmpty())
    {
        juce::Logger::writeToLog("Error: Cannot save preset with empty name");
        return;
    }
    
    auto presetFile = getPresetFile(presetName);
    
    juce::String presetContent;
    presetContent += "[Audio Processor Preset]\n";
    presetContent += "Name=" + presetName + "\n";
    presetContent += "Description=User created preset\n";
    presetContent += "\n";
    
    presetContent += "[Input]\n";
    presetContent += "Gain=" + juce::String(inputGainSlider.getValue(), 2) + "\n";
    presetContent += "\n";
    
    presetContent += "[NoiseGate]\n";
    presetContent += "Enabled=true\n";
    presetContent += "Threshold=" + juce::String(gateThresholdSlider.getValue(), 2) + "\n";
    presetContent += "Ratio=" + juce::String(gateRatioSlider.getValue(), 2) + "\n";
    presetContent += "Attack=" + juce::String(gateAttackSlider.getValue(), 2) + "\n";
    presetContent += "Release=" + juce::String(gateReleaseSlider.getValue(), 2) + "\n";
    presetContent += "\n";
    
    presetContent += "[Compressor]\n";
    presetContent += "Enabled=true\n";
    presetContent += "Threshold=" + juce::String(thresholdSlider.getValue(), 2) + "\n";
    presetContent += "Ratio=" + juce::String(ratioSlider.getValue(), 2) + "\n";
    presetContent += "Attack=" + juce::String(attackSlider.getValue(), 2) + "\n";
    presetContent += "Release=" + juce::String(releaseSlider.getValue(), 2) + "\n";
    presetContent += "Knee=" + juce::String(kneeSlider.getValue(), 2) + "\n";
    presetContent += "\n";
    
    presetContent += "[Limiter]\n";
    presetContent += "Enabled=true\n";
    presetContent += "Ceiling=" + juce::String(ceilingSlider.getValue(), 2) + "\n";
    presetContent += "Lookahead=" + juce::String(lookaheadSlider.getValue(), 2) + "\n";
    presetContent += "Release=" + juce::String(releaseSlider.getValue(), 2) + "\n";
    presetContent += "\n";
    
    presetContent += "[Output]\n";
    presetContent += "Gain=" + juce::String(outputGainSlider.getValue(), 2) + "\n";
    presetContent += "MakeupGain=" + juce::String(makeupGainSlider.getValue(), 2) + "\n";
    presetContent += "\n";
    
    // Ensure parent directory exists before saving
    auto parentDir = presetFile.getParentDirectory();
    if (!parentDir.exists())
    {
        if (!parentDir.createDirectory())
        {
            juce::Logger::writeToLog("Error: Failed to create preset directory before saving: " + parentDir.getFullPathName());
            return;
        }
    }
    
    if (presetFile.replaceWithText(presetContent))
    {
        juce::Logger::writeToLog("Preset saved successfully: " + presetFile.getFullPathName());
    }
    else
    {
        juce::Logger::writeToLog("Error saving preset: " + presetFile.getFullPathName());
        // Log more specific error information
        if (!presetFile.hasWriteAccess())
            juce::Logger::writeToLog("Error: No write access to preset file location");
        if (presetFile.isDirectory())
            juce::Logger::writeToLog("Error: Preset file path is a directory, not a file");
    }
}

void MainComponent::refreshDeviceLists()
{
    inputDeviceBox.clear();
    outputDeviceBox.clear();

    // Enumerate all device types (e.g., WASAPI, ASIO if enabled) and split capture/render
    auto& types = deviceManager.getAvailableDeviceTypes();

    juce::StringArray inputs, outputs;
    for (auto* type : types)
    {
        type->scanForDevices();
        inputs .addArray(type->getDeviceNames(true));   // capture
        outputs.addArray(type->getDeviceNames(false));  // render
    }

    inputs.removeEmptyStrings(true);
    outputs.removeEmptyStrings(true);
    inputs.removeDuplicates(true);
    outputs.removeDuplicates(true);

    inputDeviceBox .addItemList(inputs,  1);
    outputDeviceBox.addItemList(outputs, 1);

    if (inputs.size()  > 0) inputDeviceBox .setSelectedId(1, juce::dontSendNotification);
    if (outputs.size() > 0) outputDeviceBox.setSelectedId(1, juce::dontSendNotification);
}

void MainComponent::setInputDevice(const juce::String& name)
{
    auto setup = deviceManager.getAudioDeviceSetup();
    setup.inputDeviceName = name;
    deviceManager.setAudioDeviceSetup(setup, true);
}

void MainComponent::setOutputDevice(const juce::String& name)
{
    auto setup = deviceManager.getAudioDeviceSetup();
    setup.outputDeviceName = name;
    deviceManager.setAudioDeviceSetup(setup, true);
}

void MainComponent::toggleProcessing()
{
    if (!processingOn)
    {
        deviceManager.addAudioCallback(&engine);
        processingOn = true;
        enableButton.setButtonText("Disable Processing");
        juce::Logger::writeToLog("Engine ON");
    }
    else
    {
        deviceManager.removeAudioCallback(&engine);
        processingOn = false;
        enableButton.setButtonText("Enable Processing");
        juce::Logger::writeToLog("Engine OFF");
    }
}

void MainComponent::timerCallback()
{
    // Pull latest levels from the audio thread; bind these to your meter widgets.
    lastIn  = engine.inPeak.load();
    lastOut = engine.outPeak.load();
    lastGR  = engine.grDb.load();

    // Update meters with clamped values
    float clampedIn  = juce::jlimit(0.0f, 1.0f, lastIn);
    float clampedOut = juce::jlimit(0.0f, 1.0f, lastOut);

    inputMeter->setValue(clampedIn);
    outputMeter->setValue(clampedOut);
    gainReductionMeter->setValue(lastGR / 20.0f); // Normalize to 0-1 range

    repaint();
}

bool MainComponent::loadPresetFromFile(const juce::String& presetName)
{
    auto presetFile = getPresetFile(presetName);
    return loadPresetFromFile(presetName, presetFile);
}

bool MainComponent::loadPresetFromFile(const juce::String& presetName, const juce::File& presetFile)
{
    if (!presetFile.exists())
    {
        juce::Logger::writeToLog("Preset file not found: " + presetFile.getFullPathName());
        return false;
    }
    
    if (!presetFile.existsAsFile())
    {
        juce::Logger::writeToLog("Preset path is not a file: " + presetFile.getFullPathName());
        return false;
    }
    
    auto fileContent = presetFile.loadFileAsString();
    if (fileContent.isEmpty())
    {
        juce::Logger::writeToLog("Preset file is empty or could not be read: " + presetFile.getFullPathName());
        return false;
    }
    auto lines = juce::StringArray::fromLines(fileContent);
    
    juce::String currentSection;
    
    for (auto line : lines)
    {
        line = line.trim();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("#") || line.startsWith(";"))
            continue;
            
        // Check for section headers
        if (line.startsWith("[") && line.endsWith("]"))
        {
            currentSection = line.substring(1, line.length() - 1);
            continue;
        }
        
        // Parse key=value pairs
        auto equalsIndex = line.indexOfChar('=');
        if (equalsIndex == -1)
            continue;
            
        auto key = line.substring(0, equalsIndex).trim();
        auto value = line.substring(equalsIndex + 1).trim();
        
        // Apply values based on section and key with validation and clamping
        if (currentSection == "Input")
        {
            if (key == "Gain")
            {
                double val = juce::jlimit(inputGainSlider.getMinimum(), inputGainSlider.getMaximum(), value.getDoubleValue());
                inputGainSlider.setValue(val);
            }
        }
        else if (currentSection == "NoiseGate")
        {
            if (key == "Threshold")
            {
                double val = juce::jlimit(gateThresholdSlider.getMinimum(), gateThresholdSlider.getMaximum(), value.getDoubleValue());
                gateThresholdSlider.setValue(val);
            }
            else if (key == "Ratio")
            {
                double val = juce::jlimit(gateRatioSlider.getMinimum(), gateRatioSlider.getMaximum(), value.getDoubleValue());
                gateRatioSlider.setValue(val);
            }
            else if (key == "Attack")
            {
                double val = juce::jlimit(gateAttackSlider.getMinimum(), gateAttackSlider.getMaximum(), value.getDoubleValue());
                gateAttackSlider.setValue(val);
            }
            else if (key == "Release")
            {
                double val = juce::jlimit(gateReleaseSlider.getMinimum(), gateReleaseSlider.getMaximum(), value.getDoubleValue());
                gateReleaseSlider.setValue(val);
            }
        }
        else if (currentSection == "Compressor")
        {
            if (key == "Threshold")
            {
                double val = juce::jlimit(thresholdSlider.getMinimum(), thresholdSlider.getMaximum(), value.getDoubleValue());
                thresholdSlider.setValue(val);
            }
            else if (key == "Ratio")
            {
                double val = juce::jlimit(ratioSlider.getMinimum(), ratioSlider.getMaximum(), value.getDoubleValue());
                ratioSlider.setValue(val);
            }
            else if (key == "Attack")
            {
                double val = juce::jlimit(attackSlider.getMinimum(), attackSlider.getMaximum(), value.getDoubleValue());
                attackSlider.setValue(val);
            }
            else if (key == "Release")
            {
                double val = juce::jlimit(releaseSlider.getMinimum(), releaseSlider.getMaximum(), value.getDoubleValue());
                releaseSlider.setValue(val);
            }
            else if (key == "Knee")
            {
                double val = juce::jlimit(kneeSlider.getMinimum(), kneeSlider.getMaximum(), value.getDoubleValue());
                kneeSlider.setValue(val);
            }
        }
        else if (currentSection == "Limiter")
        {
            if (key == "Ceiling")
            {
                double val = juce::jlimit(ceilingSlider.getMinimum(), ceilingSlider.getMaximum(), value.getDoubleValue());
                ceilingSlider.setValue(val);
            }
            else if (key == "Lookahead")
            {
                double val = juce::jlimit(lookaheadSlider.getMinimum(), lookaheadSlider.getMaximum(), value.getDoubleValue());
                lookaheadSlider.setValue(val);
            }
            else if (key == "Release")
                ; // Limiter release is handled by releaseSlider which is already set in Compressor section
        }
        else if (currentSection == "Output")
        {
            if (key == "Gain")
            {
                double val = juce::jlimit(outputGainSlider.getMinimum(), outputGainSlider.getMaximum(), value.getDoubleValue());
                outputGainSlider.setValue(val);
            }
            else if (key == "MakeupGain")
            {
                double val = juce::jlimit(makeupGainSlider.getMinimum(), makeupGainSlider.getMaximum(), value.getDoubleValue());
                makeupGainSlider.setValue(val);
            }
        }
    }
    
    juce::Logger::writeToLog("Loaded preset from file: " + presetFile.getFullPathName());
    return true;
}

juce::File MainComponent::getPresetFile(const juce::String& presetName)
{
    // Get the appropriate directory for application data
    auto appDataDir = getPresetDirectory();
    
    // Ensure directory exists with error checking
    if (!appDataDir.exists())
    {
        if (!appDataDir.createDirectory())
        {
            juce::Logger::writeToLog("Error: Failed to create preset directory: " + appDataDir.getFullPathName());
            // Fallback to temp directory if main directory creation fails
            appDataDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                            .getChildFile("AudioBooster").getChildFile("Presets");
            if (!appDataDir.exists() && !appDataDir.createDirectory())
            {
                juce::Logger::writeToLog("Error: Failed to create fallback preset directory: " + appDataDir.getFullPathName());
            }
        }
    }
    
    return appDataDir.getChildFile(presetName + ".preset");
}

void MainComponent::refreshPresetList()
{
    juce::String currentSelection = presetBox.getText();
    presetBox.clear();
    
    // Add default hardcoded presets
    presetBox.addItem("Default", 1);
    presetBox.addItem("Podcast", 2);
    presetBox.addItem("Streaming", 3);
    presetBox.addItem("VoiceOver", 4);
    presetBox.addItem("SlammedUp", 5);
    
    // Scan for .preset files in the preset directory
    auto presetDir = getPresetDirectory();
    if (presetDir.exists())
    {
        auto presetFiles = presetDir.findChildFiles(juce::File::findFiles, false, "*.preset");
        
        int itemId = 6; // Start after hardcoded presets
        for (const auto& file : presetFiles)
        {
            juce::String presetName = file.getFileNameWithoutExtension();
            
            // Skip if it's already a hardcoded preset
            if (isBuiltInPreset(presetName))
                continue;
                
            presetBox.addItem(presetName, itemId++);
        }
    }
    
    // Try to restore previous selection
    if (currentSelection.isNotEmpty())
    {
        for (int i = 0; i < presetBox.getNumItems(); ++i)
        {
            if (presetBox.getItemText(i) == currentSelection)
            {
                presetBox.setSelectedItemIndex(i, juce::dontSendNotification);
                return;
            }
        }
    }
    
    // Default to first item if previous selection not found
    presetBox.setSelectedId(1, juce::dontSendNotification);
}

juce::File MainComponent::getPresetDirectory()
{
    // Get platform-appropriate application data directory
    auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    return appDataDir.getChildFile("AudioBooster").getChildFile("Presets");
}

bool MainComponent::isBuiltInPreset(const juce::String& presetName)
{
    return (presetName == "Default" || presetName == "Podcast" || 
            presetName == "Streaming" || presetName == "VoiceOver" || 
            presetName == "SlammedUp");
}

juce::String MainComponent::cleanPresetName(const juce::String& name)
{
    // Remove invalid characters for file names
    juce::String cleaned = name;
    
    // Remove or replace invalid characters
    cleaned = cleaned.replaceCharacters("\\/:*?\"<>|", "");
    
    // Trim whitespace
    cleaned = cleaned.trim();
    
    // Ensure it's not empty after cleaning
    if (cleaned.isEmpty())
        return juce::String();
        
    return cleaned;
}

void MainComponent::doSavePreset(const juce::String& presetName, const juce::File& presetFile)
{
    // Save the preset
    savePreset(presetName);
    
    if (presetFile.exists())
    {
        refreshPresetList();
        
        // Select the newly saved preset
        for (int i = 0; i < presetBox.getNumItems(); ++i)
        {
            if (presetBox.getItemText(i) == presetName)
            {
                presetBox.setSelectedItemIndex(i, juce::dontSendNotification);
                break;
            }
        }
        
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Preset Saved",
            "Preset saved successfully: " + presetName
        );
    }
    else
    {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Save Failed",
            "Failed to save preset file: " + presetName
        );
    }
}

void MainComponent::handleSavePresetClick()
{
    // Show input dialog for custom preset name
    juce::String currentSelection = presetBox.getText();
    juce::String defaultName = currentSelection.isNotEmpty() && !isBuiltInPreset(currentSelection) 
        ? currentSelection : "MyPreset";
    
    // Use async AlertWindow for text input
    auto* alertWindow = new juce::AlertWindow("Save Preset", "Enter a name for your preset:", juce::AlertWindow::QuestionIcon);
    alertWindow->addTextEditor("presetName", defaultName, "Preset name:");
    alertWindow->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
    
    alertWindow->enterModalState(true, juce::ModalCallbackFunction::create([this, alertWindow](int result)
    {
        std::unique_ptr<juce::AlertWindow> windowDeleter(alertWindow);
        
        if (result == 0) // User cancelled
            return;
            
        juce::String presetName = alertWindow->getTextEditorContents("presetName");
        
        // Check if user cancelled or entered empty name
        if (presetName.isEmpty())
        {
            return;
        }
        
        // Clean the name (remove invalid characters)
        presetName = cleanPresetName(presetName);
        
        if (presetName.isEmpty())
        {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Invalid Name",
                "Please enter a valid preset name."
            );
            return;
        }
        
        // Check if it's a built-in preset name
        if (isBuiltInPreset(presetName))
        {
            juce::NativeMessageBox::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon,
                "Cannot Use Name",
                "Cannot use built-in preset names. Please choose a different name."
            );
            return;
        }
        
        auto presetFile = getPresetFile(presetName);
        
        // Check if file already exists and ask for confirmation
        if (presetFile.exists())
        {
            auto* confirmWindow = new juce::AlertWindow("Preset Exists", 
                "A preset with this name already exists. Do you want to overwrite it?", 
                juce::AlertWindow::QuestionIcon);
            confirmWindow->addButton("Yes", 1, juce::KeyPress(juce::KeyPress::returnKey));
            confirmWindow->addButton("No", 0, juce::KeyPress(juce::KeyPress::escapeKey));
            
            confirmWindow->enterModalState(true, juce::ModalCallbackFunction::create([this, confirmWindow, presetName, presetFile](int overwriteResult)
            {
                std::unique_ptr<juce::AlertWindow> windowDeleter(confirmWindow);
                
                if (overwriteResult == 0) // User said No
                    return;
                    
                // Continue with save process
                this->doSavePreset(presetName, presetFile);
            }));
        }
        else
        {
            // File doesn't exist, save directly
            this->doSavePreset(presetName, presetFile);
        }
    }));
}

void MainComponent::handleLoadPresetClick()
{
    // Refresh the preset list to show any new .preset files that might have been added
    refreshPresetList();
    
    juce::NativeMessageBox::showMessageBoxAsync(
        juce::AlertWindow::InfoIcon,
        "Preset List Refreshed",
        "Preset list has been refreshed. Select a preset from the dropdown to load it."
    );
}

void MainComponent::handleDeletePresetClick()
{
    juce::String currentSelection = presetBox.getText();
    
    if (currentSelection.isEmpty())
    {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "No Preset Selected",
            "Please select a preset to delete."
        );
        return;
    }
    
    // Don't allow deleting built-in presets
    if (isBuiltInPreset(currentSelection))
    {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Cannot Delete",
            "Built-in presets cannot be deleted. Only custom presets can be removed."
        );
        return;
    }
    
    auto presetFile = getPresetFile(currentSelection);
    if (!presetFile.exists())
    {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Preset Not Found",
            "The preset file does not exist: " + presetFile.getFullPathName()
        );
        return;
    }
    
    // Delete the preset file
    if (presetFile.deleteFile())
    {
        refreshPresetList();
        
        // Select first preset after deletion
        if (presetBox.getNumItems() > 0)
        {
            presetBox.setSelectedItemIndex(0, juce::dontSendNotification);
            loadPreset(presetBox.getText());
        }
        
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon,
            "Preset Deleted",
            "Preset deleted successfully: " + currentSelection
        );
    }
    else
    {
        juce::NativeMessageBox::showMessageBoxAsync(
            juce::AlertWindow::WarningIcon,
            "Delete Failed",
            "Failed to delete preset file: " + presetFile.getFullPathName()
        );
    }
}
