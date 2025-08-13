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

    addAndMakeVisible(inputMeter.get());
    addAndMakeVisible(outputMeter.get());
    addAndMakeVisible(gainReductionMeter.get());

    // Setup sliders and labels
    setupSliders();
    setupLabels();
    setupPresets();

    // Setup callbacks
    inputDeviceBox.onChange  = [this]{ setInputDevice (inputDeviceBox.getText());  };
    outputDeviceBox.onChange = [this]{ setOutputDevice(outputDeviceBox.getText()); };
    enableButton.onClick     = [this]{ toggleProcessing(); };

    savePresetButton.onClick = [this]{ savePreset(presetBox.getText()); };
    loadPresetButton.onClick = [this]{ loadPreset(presetBox.getText()); };
    presetBox.onChange = [this]{ loadPreset(presetBox.getText()); };

    refreshDeviceLists();
    startTimerHz(30); // meter refresh
}

MainComponent::~MainComponent()
{
    if (processingOn)
        deviceManager.removeAudioCallback(&engine);
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

    // Title area
    auto titleArea = bounds.removeFromTop(40);

    // Device controls area
    auto deviceArea = bounds.removeFromTop(100);
    inputDeviceBox.setBounds(deviceArea.removeFromLeft(deviceArea.getWidth() / 2 - 5).removeFromTop(25));
    outputDeviceBox.setBounds(deviceArea.removeFromRight(deviceArea.getWidth() / 2 - 5).removeFromTop(25));
    enableButton.setBounds(deviceArea.removeFromTop(30).reduced(0, 5));

    // Meters area (right side)
    auto metersArea = bounds.removeFromRight(80);
    inputMeter->setBounds(metersArea.removeFromTop(120));
    metersArea.removeFromTop(10);
    outputMeter->setBounds(metersArea.removeFromTop(120));
    metersArea.removeFromTop(10);
    gainReductionMeter->setBounds(metersArea.removeFromTop(120));

    // Preset area
    auto presetArea = bounds.removeFromTop(60);
    presetBox.setBounds(presetArea.removeFromLeft(presetArea.getWidth() / 2 - 5).removeFromTop(25));
    auto buttonArea = presetArea.removeFromRight(presetArea.getWidth() / 2 - 5);
    savePresetButton.setBounds(buttonArea.removeFromTop(25));
    buttonArea.removeFromTop(5);
    loadPresetButton.setBounds(buttonArea.removeFromTop(25));

    // Controls area (left side) - organized in columns
    auto controlsArea = bounds;

    // Column 1: Input/Output gains
    auto col1 = controlsArea.removeFromLeft(controlsArea.getWidth() / 4);
    inputGainLabel.setBounds(col1.removeFromTop(20));
    inputGainSlider.setBounds(col1.removeFromTop(80));
    col1.removeFromTop(10);
    outputGainLabel.setBounds(col1.removeFromTop(20));
    outputGainSlider.setBounds(col1.removeFromTop(80));

    // Column 2: Noise Gate
    auto col2 = controlsArea.removeFromLeft(controlsArea.getWidth() / 3);
    gateThresholdLabel.setBounds(col2.removeFromTop(20));
    gateThresholdSlider.setBounds(col2.removeFromTop(80));
    col2.removeFromTop(10);
    gateRatioLabel.setBounds(col2.removeFromTop(20));
    gateRatioSlider.setBounds(col2.removeFromTop(80));
    col2.removeFromTop(10);
    gateAttackLabel.setBounds(col2.removeFromTop(20));
    gateAttackSlider.setBounds(col2.removeFromTop(80));
    col2.removeFromTop(10);
    gateReleaseLabel.setBounds(col2.removeFromTop(20));
    gateReleaseSlider.setBounds(col2.removeFromTop(80));

    // Column 3: Compressor
    auto col3 = controlsArea.removeFromLeft(controlsArea.getWidth() / 2);
    thresholdLabel.setBounds(col3.removeFromTop(20));
    thresholdSlider.setBounds(col3.removeFromTop(80));
    col3.removeFromTop(10);
    ratioLabel.setBounds(col3.removeFromTop(20));
    ratioSlider.setBounds(col3.removeFromTop(80));
    col3.removeFromTop(10);
    attackLabel.setBounds(col3.removeFromTop(20));
    attackSlider.setBounds(col3.removeFromTop(80));
    col3.removeFromTop(10);
    releaseLabel.setBounds(col3.removeFromTop(20));
    releaseSlider.setBounds(col3.removeFromTop(80));

    // Column 4: Limiter and additional controls
    auto col4 = controlsArea;
    ceilingLabel.setBounds(col4.removeFromTop(20));
    ceilingSlider.setBounds(col4.removeFromTop(80));
    col4.removeFromTop(10);
    lookaheadLabel.setBounds(col4.removeFromTop(20));
    lookaheadSlider.setBounds(col4.removeFromTop(80));
    col4.removeFromTop(10);
    kneeLabel.setBounds(col4.removeFromTop(20));
    kneeSlider.setBounds(col4.removeFromTop(80));
    col4.removeFromTop(10);
    makeupGainLabel.setBounds(col4.removeFromTop(20));
    makeupGainSlider.setBounds(col4.removeFromTop(80));
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
    presetBox.addItem("Default", 1);
    presetBox.addItem("Podcast", 2);
    presetBox.addItem("Streaming", 3);
    presetBox.addItem("VoiceOver", 4);
    presetBox.setSelectedId(1, juce::dontSendNotification);
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

    updateEngineParameters();
}

void MainComponent::savePreset(const juce::String& presetName)
{
    // This would save to a file in a real implementation
    juce::Logger::writeToLog("Saving preset: " + presetName);
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
