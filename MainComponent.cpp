#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Initialize audio engine
    audioEngine = std::make_unique<AudioEngine>();
    
    // Setup GUI components
    addAndMakeVisible(inputGroup);
    inputGroup.setText("Input");
    inputGroup.setTextLabelPosition(juce::Justification::centredTop);
    
    addAndMakeVisible(processingGroup);
    processingGroup.setText("Processing");
    processingGroup.setTextLabelPosition(juce::Justification::centredTop);
    
    addAndMakeVisible(outputGroup);
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::centredTop);
    
    // Input controls
    setupSlider(inputGainSlider, inputGainLabel, "Input Gain", -40.0, 40.0, 0.0, " dB");
    setupComboBox(inputDeviceCombo, inputDeviceLabel, "Input Device");
    
    // Compressor controls
    setupSlider(compressorThresholdSlider, compressorThresholdLabel, "Threshold", -60.0, 0.0, -20.0, " dB");
    setupSlider(compressorRatioSlider, compressorRatioLabel, "Ratio", 1.0, 20.0, 4.0, ":1");
    setupSlider(compressorAttackSlider, compressorAttackLabel, "Attack", 0.1, 100.0, 1.0, " ms");
    setupSlider(compressorReleaseSlider, compressorReleaseLabel, "Release", 1.0, 1000.0, 30.0, " ms");
    
    // Limiter controls
    setupSlider(limiterCeilingSlider, limiterCeilingLabel, "Ceiling", -10.0, 0.0, -0.3, " dB");
    setupSlider(limiterLookaheadSlider, limiterLookaheadLabel, "Lookahead", 0.1, 10.0, 3.0, " ms");
    
    // Output controls
    setupSlider(outputGainSlider, outputGainLabel, "Output Gain", -40.0, 40.0, 0.0, " dB");
    setupComboBox(outputDeviceCombo, outputDeviceLabel, "Output Device");
    
    // Control buttons
    addAndMakeVisible(enableButton);
    enableButton.setButtonText("Enable Processing");
    enableButton.setToggleable(true);
    enableButton.onClick = [this]() {
        audioEngine->setEnabled(enableButton.getToggleState());
        enableButton.setButtonText(enableButton.getToggleState() ? "Disable Processing" : "Enable Processing");
    };
    
    // Preset management
    setupComboBox(presetCombo, presetLabel, "Preset");
    presetCombo.addItem("Default", 1);
    presetCombo.addItem("Podcast", 2);
    presetCombo.addItem("Streaming", 3);
    presetCombo.addItem("Voice Over", 4);
    presetCombo.setSelectedId(1);
    presetCombo.onChange = [this]() { loadPreset(presetCombo.getSelectedId()); };
    
    addAndMakeVisible(presetButton);
    presetButton.setButtonText("Save Preset");
    presetButton.onClick = [this]() { saveCurrentAsPreset(); };
    
    // Setup parameter listeners
    inputGainSlider.onValueChange = [this]() {
        audioEngine->setInputGain(static_cast<float>(inputGainSlider.getValue()));
    };
    
    compressorThresholdSlider.onValueChange = [this]() {
        audioEngine->setCompressorThreshold(static_cast<float>(compressorThresholdSlider.getValue()));
    };
    
    compressorRatioSlider.onValueChange = [this]() {
        audioEngine->setCompressorRatio(static_cast<float>(compressorRatioSlider.getValue()));
    };
    
    compressorAttackSlider.onValueChange = [this]() {
        audioEngine->setCompressorAttack(static_cast<float>(compressorAttackSlider.getValue()));
    };
    
    compressorReleaseSlider.onValueChange = [this]() {
        audioEngine->setCompressorRelease(static_cast<float>(compressorReleaseSlider.getValue()));
    };
    
    limiterCeilingSlider.onValueChange = [this]() {
        audioEngine->setLimiterCeiling(static_cast<float>(limiterCeilingSlider.getValue()));
    };
    
    limiterLookaheadSlider.onValueChange = [this]() {
        audioEngine->setLimiterLookahead(static_cast<float>(limiterLookaheadSlider.getValue()));
    };
    
    outputGainSlider.onValueChange = [this]() {
        audioEngine->setOutputGain(static_cast<float>(outputGainSlider.getValue()));
    };
    
    // Update device lists
    updateDeviceLists();
    
    // Set window size
    setSize(800, 600);
    
    // Start timer for level meter updates
    startTimer(50); // 20 FPS
    
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    audioEngine->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    audioEngine->processAudioBlock(*bufferToFill.buffer, bufferToFill.startSample, bufferToFill.numSamples);
    
    // Update level meters
    inputLevel = bufferToFill.buffer->getMagnitude(0, bufferToFill.numSamples);
    outputLevel = bufferToFill.buffer->getMagnitude(0, bufferToFill.numSamples);
    gainReduction = audioEngine->getCurrentGainReduction();
}

void MainComponent::releaseResources()
{
    audioEngine->releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // Draw level meters
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(inputMeterBounds);
    g.fillRect(outputMeterBounds);
    g.fillRect(grMeterBounds);
    
    // Draw meter levels
    g.setColour(juce::Colours::green);
    auto inputHeight = static_cast<int>(inputLevel * inputMeterBounds.getHeight());
    g.fillRect(inputMeterBounds.getX(), inputMeterBounds.getBottom() - inputHeight, 
               inputMeterBounds.getWidth(), inputHeight);
    
    auto outputHeight = static_cast<int>(outputLevel * outputMeterBounds.getHeight());
    g.fillRect(outputMeterBounds.getX(), outputMeterBounds.getBottom() - outputHeight,
               outputMeterBounds.getWidth(), outputHeight);
    
    g.setColour(juce::Colours::red);
    auto grHeight = static_cast<int>(gainReduction * grMeterBounds.getHeight());
    g.fillRect(grMeterBounds.getX(), grMeterBounds.getY(),
               grMeterBounds.getWidth(), grHeight);
    
    // Draw meter labels
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText("IN", inputMeterBounds.getX(), inputMeterBounds.getBottom() + 5, 
               inputMeterBounds.getWidth(), 15, juce::Justification::centred);
    g.drawText("OUT", outputMeterBounds.getX(), outputMeterBounds.getBottom() + 5,
               outputMeterBounds.getWidth(), 15, juce::Justification::centred);
    g.drawText("GR", grMeterBounds.getX(), grMeterBounds.getBottom() + 5,
               grMeterBounds.getWidth(), 15, juce::Justification::centred);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    auto margin = 10;
    
    // Top row - presets and enable button
    auto topRow = bounds.removeFromTop(40);
    presetLabel.setBounds(topRow.removeFromLeft(60));
    presetCombo.setBounds(topRow.removeFromLeft(150));
    topRow.removeFromLeft(margin);
    presetButton.setBounds(topRow.removeFromLeft(100));
    topRow.removeFromLeft(margin);
    enableButton.setBounds(topRow.removeFromLeft(150));
    
    bounds.removeFromTop(margin);
    
    // Main content area
    auto contentHeight = bounds.getHeight() / 3;
    
    // Input section
    auto inputBounds = bounds.removeFromTop(contentHeight);
    inputGroup.setBounds(inputBounds);
    inputBounds.reduce(margin, margin + 20);
    
    auto inputLeft = inputBounds.removeFromLeft(inputBounds.getWidth() / 2);
    inputDeviceLabel.setBounds(inputLeft.removeFromTop(20));
    inputDeviceCombo.setBounds(inputLeft.removeFromTop(25));
    inputLeft.removeFromTop(margin);
    inputGainLabel.setBounds(inputLeft.removeFromTop(20));
    inputGainSlider.setBounds(inputLeft.removeFromTop(25));
    
    // Input meter
    inputMeterBounds = inputBounds.removeFromRight(30);
    inputMeterBounds.removeFromTop(20);
    inputMeterBounds.removeFromBottom(20);
    
    // Processing section
    auto processingBounds = bounds.removeFromTop(contentHeight);
    processingGroup.setBounds(processingBounds);
    processingBounds.reduce(margin, margin + 20);
    
    auto compressorBounds = processingBounds.removeFromLeft(processingBounds.getWidth() / 2);
    auto limiterBounds = processingBounds;
    
    // Compressor controls
    compressorThresholdLabel.setBounds(compressorBounds.removeFromTop(20));
    compressorThresholdSlider.setBounds(compressorBounds.removeFromTop(25));
    compressorRatioLabel.setBounds(compressorBounds.removeFromTop(20));
    compressorRatioSlider.setBounds(compressorBounds.removeFromTop(25));
    compressorAttackLabel.setBounds(compressorBounds.removeFromTop(20));
    compressorAttackSlider.setBounds(compressorBounds.removeFromTop(25));
    compressorReleaseLabel.setBounds(compressorBounds.removeFromTop(20));
    compressorReleaseSlider.setBounds(compressorBounds.removeFromTop(25));
    
    // Limiter controls
    limiterBounds.removeFromLeft(margin);
    limiterCeilingLabel.setBounds(limiterBounds.removeFromTop(20));
    limiterCeilingSlider.setBounds(limiterBounds.removeFromTop(25));
    limiterLookaheadLabel.setBounds(limiterBounds.removeFromTop(20));
    limiterLookaheadSlider.setBounds(limiterBounds.removeFromTop(25));
    
    // Gain reduction meter
    grMeterBounds = juce::Rectangle<int>(processingBounds.getRight() - 30, processingBounds.getY() + 20,
                                         30, processingBounds.getHeight() - 40);
    
    // Output section
    auto outputBounds = bounds;
    outputGroup.setBounds(outputBounds);
    outputBounds.reduce(margin, margin + 20);
    
    auto outputLeft = outputBounds.removeFromLeft(outputBounds.getWidth() / 2);
    outputDeviceLabel.setBounds(outputLeft.removeFromTop(20));
    outputDeviceCombo.setBounds(outputLeft.removeFromTop(25));
    outputLeft.removeFromTop(margin);
    outputGainLabel.setBounds(outputLeft.removeFromTop(20));
    outputGainSlider.setBounds(outputLeft.removeFromTop(25));
    
    // Output meter
    outputMeterBounds = outputBounds.removeFromRight(30);
    outputMeterBounds.removeFromTop(20);
    outputMeterBounds.removeFromBottom(20);
}

void MainComponent::timerCallback()
{
    repaint();
}

//==============================================================================
void MainComponent::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText,
                                 double min, double max, double defaultValue, const juce::String& suffix)
{
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
    
    slider.setRange(min, max);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    slider.setTextValueSuffix(suffix);
    
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
}

void MainComponent::setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(combo);
    addAndMakeVisible(label);
    
    label.setText(labelText, juce::dontSendNotification);
    label.attachToComponent(&combo, false);
}

void MainComponent::updateDeviceLists()
{
    auto& audioDeviceManager = deviceManager;
    
    inputDeviceCombo.clear();
    outputDeviceCombo.clear();
    
    auto& inputDevices = audioDeviceManager.getAvailableDeviceTypes();
    for (auto* deviceType : inputDevices)
    {
        auto inputNames = deviceType->getDeviceNames(true);
        for (auto& name : inputNames)
        {
            inputDeviceCombo.addItem(name, inputDeviceCombo.getNumItems() + 1);
        }
        
        auto outputNames = deviceType->getDeviceNames(false);
        for (auto& name : outputNames)
        {
            outputDeviceCombo.addItem(name, outputDeviceCombo.getNumItems() + 1);
        }
    }
}

void MainComponent::loadPreset(int presetIndex)
{
    switch (presetIndex)
    {
        case 1: // Default
            inputGainSlider.setValue(0.0);
            compressorThresholdSlider.setValue(-20.0);
            compressorRatioSlider.setValue(4.0);
            compressorAttackSlider.setValue(1.0);
            compressorReleaseSlider.setValue(30.0);
            limiterCeilingSlider.setValue(-0.3);
            limiterLookaheadSlider.setValue(3.0);
            outputGainSlider.setValue(0.0);
            break;
            
        case 2: // Podcast
            inputGainSlider.setValue(6.0);
            compressorThresholdSlider.setValue(-18.0);
            compressorRatioSlider.setValue(3.0);
            compressorAttackSlider.setValue(2.0);
            compressorReleaseSlider.setValue(50.0);
            limiterCeilingSlider.setValue(-1.0);
            limiterLookaheadSlider.setValue(5.0);
            outputGainSlider.setValue(2.0);
            break;
            
        case 3: // Streaming
            inputGainSlider.setValue(3.0);
            compressorThresholdSlider.setValue(-15.0);
            compressorRatioSlider.setValue(6.0);
            compressorAttackSlider.setValue(0.5);
            compressorReleaseSlider.setValue(20.0);
            limiterCeilingSlider.setValue(-0.1);
            limiterLookaheadSlider.setValue(2.0);
            outputGainSlider.setValue(1.0);
            break;
            
        case 4: // Voice Over
            inputGainSlider.setValue(8.0);
            compressorThresholdSlider.setValue(-25.0);
            compressorRatioSlider.setValue(2.5);
            compressorAttackSlider.setValue(3.0);
            compressorReleaseSlider.setValue(100.0);
            limiterCeilingSlider.setValue(-2.0);
            limiterLookaheadSlider.setValue(8.0);
            outputGainSlider.setValue(3.0);
            break;
    }
}

void MainComponent::saveCurrentAsPreset()
{
    // This would typically open a dialog to save the current settings as a new preset
    // For now, we'll just show an alert
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                           "Save Preset",
                                           "Preset saving functionality would be implemented here.");
}

