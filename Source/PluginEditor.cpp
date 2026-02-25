#include "PluginEditor.h"
#include "BinaryData.h"

BombFXAudioProcessorEditor::BombFXAudioProcessorEditor(BombFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setSize(1100, 550);
    
    // Load logo from binary data
    logoImage = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    
    // Apply custom look and feel
    setLookAndFeel(&customLookAndFeel);
    
    // Preset selector
    presetLabel.setText("PRESET:", juce::dontSendNotification);
    presetLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    presetLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(presetLabel);
    
    presetSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2a2a3e));
    presetSelector.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    presetSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff8b5cf6));
    presetSelector.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff8b5cf6));
    addAndMakeVisible(presetSelector);
    populatePresets();
    loadUserPresets(); // Load user presets on startup
    presetSelector.onChange = [this] {
        int selectedId = presetSelector.getSelectedId();
        
        // Skip header items (IDs 100, 200, 300, 400, 500)
        if (selectedId > 0 && selectedId < 100) {
            juce::String selectedText = presetSelector.getText();
            loadPreset(selectedText);
        }
    };
    
    // Save preset button
    savePresetButton.setButtonText("Save");
    savePresetButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff8b5cf6));
    savePresetButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    savePresetButton.onClick = [this] { saveCurrentPreset(); };
    addAndMakeVisible(savePresetButton);
    
    // Refresh user presets button
    loadUserPresetsButton.setButtonText("Refresh");
    loadUserPresetsButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a3e));
    loadUserPresetsButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff00d4ff));
    loadUserPresetsButton.onClick = [this] { loadUserPresets(); };
    addAndMakeVisible(loadUserPresetsButton);
    
    auto& params = audioProcessor.getParameters();
    
    // === REVERB ===
    reverbLabel.setText("REVERB", juce::dontSendNotification);
    reverbLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    reverbLabel.setJustificationType(juce::Justification::centred);
    reverbLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(reverbLabel);
    
    reverbMixLabel.setText("MIX", juce::dontSendNotification);
    reverbMixLabel.setFont(juce::Font(12.0f));
    reverbMixLabel.setJustificationType(juce::Justification::centred);
    reverbMixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8b5cf6));
    addAndMakeVisible(reverbMixLabel);
    
    auto setupLabel = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(11.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa));
        addAndMakeVisible(label);
    };
    
    setupLabel(reverbRoomSizeLabel, "ROOM");
    setupLabel(reverbDampingLabel, "DAMP");
    setupLabel(reverbWidthLabel, "WIDTH");
    
    auto setupSlider = [this](juce::Slider& slider, const juce::String& paramID) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);
        sliderAttachments.push_back(
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.getParameters(), paramID, slider));
    };
    
    auto setupLinearSlider = [this](juce::Slider& slider, const juce::String& paramID) {
        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::trackColourId, juce::Colour(0xff2a2a3e));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8b5cf6));
        addAndMakeVisible(slider);
        sliderAttachments.push_back(
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.getParameters(), paramID, slider));
    };
    
    setupSlider(reverbRoomSizeSlider, "reverbRoomSize");
    setupSlider(reverbDampingSlider, "reverbDamping");
    setupSlider(reverbWidthSlider, "reverbWidth");
    
    // Reverb mix fader
    reverbMixSlider.setSliderStyle(juce::Slider::LinearVertical);
    reverbMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    reverbMixSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff2a2a3e));
    reverbMixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8b5cf6));
    reverbMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    reverbMixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(reverbMixSlider);
    sliderAttachments.push_back(
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getParameters(), "reverbMix", reverbMixSlider));
    
    // === DELAY ===
    delayLabel.setText("DELAY", juce::dontSendNotification);
    delayLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    delayLabel.setJustificationType(juce::Justification::centred);
    delayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(delayLabel);
    
    delayMixLabel.setText("MIX", juce::dontSendNotification);
    delayMixLabel.setFont(juce::Font(12.0f));
    delayMixLabel.setJustificationType(juce::Justification::centred);
    delayMixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8b5cf6));
    addAndMakeVisible(delayMixLabel);
    
    setupLabel(delayTimeLabel, "TIME");
    setupLabel(delayFeedbackLabel, "FDBK");
    
    setupSlider(delayTimeSlider, "delayTime");
    setupSlider(delayFeedbackSlider, "delayFeedback");
    
    // Delay mix fader
    delayMixSlider.setSliderStyle(juce::Slider::LinearVertical);
    delayMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    delayMixSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff2a2a3e));
    delayMixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8b5cf6));
    delayMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    delayMixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(delayMixSlider);
    sliderAttachments.push_back(
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getParameters(), "delayMix", delayMixSlider));
    
    // === CHORUS ===
    chorusLabel.setText("CHORUS", juce::dontSendNotification);
    chorusLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    chorusLabel.setJustificationType(juce::Justification::centred);
    chorusLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(chorusLabel);
    
    chorusMixLabel.setText("MIX", juce::dontSendNotification);
    chorusMixLabel.setFont(juce::Font(12.0f));
    chorusMixLabel.setJustificationType(juce::Justification::centred);
    chorusMixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8b5cf6));
    addAndMakeVisible(chorusMixLabel);
    
    setupLabel(chorusRateLabel, "RATE");
    setupLabel(chorusDepthLabel, "DEPTH");
    setupLabel(chorusCenterDelayLabel, "DELAY");
    
    setupSlider(chorusRateSlider, "chorusRate");
    setupSlider(chorusDepthSlider, "chorusDepth");
    setupSlider(chorusCenterDelaySlider, "chorusCenterDelay");
    
    // Chorus mix fader
    chorusMixSlider.setSliderStyle(juce::Slider::LinearVertical);
    chorusMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    chorusMixSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff2a2a3e));
    chorusMixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8b5cf6));
    chorusMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    chorusMixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(chorusMixSlider);
    sliderAttachments.push_back(
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getParameters(), "chorusMix", chorusMixSlider));
    
    // === FILTER ===
    filterLabel.setText("FILTER", juce::dontSendNotification);
    filterLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    filterLabel.setJustificationType(juce::Justification::centred);
    filterLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(filterLabel);
    
    filterMixLabel.setText("MIX", juce::dontSendNotification);
    filterMixLabel.setFont(juce::Font(12.0f));
    filterMixLabel.setJustificationType(juce::Justification::centred);
    filterMixLabel.setColour(juce::Label::textColourId, juce::Colour(0xff8b5cf6));
    addAndMakeVisible(filterMixLabel);
    
    setupLabel(filterCutoffLabel, "CUTOFF");
    setupLabel(filterResonanceLabel, "RESO");
    setupLabel(filterDriveLabel, "DRIVE");
    setupLabel(filterTypeLabel, "TYPE");
    
    setupSlider(filterCutoffSlider, "filterCutoff");
    setupSlider(filterResonanceSlider, "filterResonance");
    setupSlider(filterDriveSlider, "filterDrive");
    
    // Filter mix fader
    filterMixSlider.setSliderStyle(juce::Slider::LinearVertical);
    filterMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    filterMixSlider.setColour(juce::Slider::trackColourId, juce::Colour(0xff2a2a3e));
    filterMixSlider.setColour(juce::Slider::thumbColourId, juce::Colour(0xff8b5cf6));
    filterMixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    filterMixSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(filterMixSlider);
    sliderAttachments.push_back(
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getParameters(), "filterMix", filterMixSlider));
    
    // Filter type selector
    filterTypeSelector.addItem("Low Pass", 1);
    filterTypeSelector.addItem("High Pass", 2);
    filterTypeSelector.addItem("Band Pass", 3);
    filterTypeSelector.setSelectedId(1);
    filterTypeSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff2a2a3e));
    filterTypeSelector.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    filterTypeSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff8b5cf6));
    filterTypeSelector.setColour(juce::ComboBox::arrowColourId, juce::Colour(0xff8b5cf6));
    addAndMakeVisible(filterTypeSelector);
    filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getParameters(), "filterType", filterTypeSelector);
}

void BombFXAudioProcessorEditor::populatePresets() {
    presetSelector.addItem("Reset", 1);
    presetSelector.addItem("--- REVERB ---", 100);
    presetSelector.addItem("Cathedral", 2);
    presetSelector.addItem("Small Room", 3);
    presetSelector.addItem("Vocal Plate", 4);
    presetSelector.addItem("Huge Hall", 5);
    presetSelector.addItem("Dark Chamber", 17);
    presetSelector.addItem("Spring Tank", 18);
    
    presetSelector.addItem("--- DELAY ---", 200);
    presetSelector.addItem("Slapback", 6);
    presetSelector.addItem("Dub Echo", 7);
    presetSelector.addItem("Ping Pong", 8);
    presetSelector.addItem("Dotted Eighth", 9);
    presetSelector.addItem("Triplet Delay", 19);
    presetSelector.addItem("Long Tail", 20);
    
    presetSelector.addItem("--- CHORUS ---", 300);
    presetSelector.addItem("Lush Chorus", 10);
    presetSelector.addItem("Subtle Shimmer", 11);
    presetSelector.addItem("Wide Stereo", 12);
    presetSelector.addItem("Studio Thickener", 13);
    presetSelector.addItem("Vibrato", 21);
    presetSelector.addItem("Ensemble", 22);
    
    presetSelector.addItem("--- COMBO ---", 400);
    presetSelector.addItem("Ambient Space", 14);
    presetSelector.addItem("Dream Machine", 15);
    presetSelector.addItem("Infinite Void", 16);
    presetSelector.addItem("Vocal Air", 23);
    presetSelector.addItem("Synth Pad", 24);
    presetSelector.addItem("Guitar Shimmer", 25);
    presetSelector.addItem("Lo-Fi Vibes", 26);
    presetSelector.addItem("Psychedelic", 27);
    presetSelector.addItem("Epic Drums", 28);
}

void BombFXAudioProcessorEditor::setParameterValue(const juce::String& paramID, float value) {
    // Use APVTS to get and set the parameter value
    auto& apvts = audioProcessor.getParameters();
    
    if (auto* param = apvts.getParameter(paramID)) {
        if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
            *floatParam = value;
        }
        else if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(param)) {
            *choiceParam = static_cast<int>(value);
        }
    }
}

void BombFXAudioProcessorEditor::loadPreset(const juce::String& presetName) {
    // Check if it's a user preset (try loading from file first)
    auto presetFile = getUserPresetsDirectory().getChildFile(presetName + ".bombfx");
    if (presetFile.existsAsFile()) {
        auto xml = juce::parseXML(presetFile);
        if (xml && xml->hasTagName("BombFXPreset")) {
            // Load all attributes as parameters
            for (int i = 0; i < xml->getNumAttributes(); ++i) {
                auto paramName = xml->getAttributeName(i);
                auto paramValue = xml->getAttributeValue(i);
                
                if (paramName != "name") {
                    setParameterValue(paramName, paramValue.getFloatValue());
                }
            }
            return;
        }
    }
    
    // Otherwise load factory preset (hardcoded)
    // Hardcoded presets - fast and reliable
    if (presetName == "Reset") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    // REVERB PRESETS
    if (presetName == "Cathedral") {
        setParameterValue("reverbMix", 0.45f);
        setParameterValue("reverbRoomSize", 0.8f);
        setParameterValue("reverbDamping", 0.3f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Small Room") {
        setParameterValue("reverbMix", 0.3f);
        setParameterValue("reverbRoomSize", 0.3f);
        setParameterValue("reverbDamping", 0.7f);
        setParameterValue("reverbWidth", 0.8f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Vocal Plate") {
        setParameterValue("reverbMix", 0.35f);
        setParameterValue("reverbRoomSize", 0.5f);
        setParameterValue("reverbDamping", 0.6f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Huge Hall") {
        setParameterValue("reverbMix", 0.55f);
        setParameterValue("reverbRoomSize", 0.95f);
        setParameterValue("reverbDamping", 0.2f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    // DELAY PRESETS
    if (presetName == "Slapback") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.35f);
        setParameterValue("delayTime", 120.0f);
        setParameterValue("delayFeedback", 0.2f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Dub Echo") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.5f);
        setParameterValue("delayTime", 375.0f);
        setParameterValue("delayFeedback", 0.7f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Ping Pong") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.4f);
        setParameterValue("delayTime", 500.0f);
        setParameterValue("delayFeedback", 0.5f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Dotted Eighth") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.45f);
        setParameterValue("delayTime", 667.0f);
        setParameterValue("delayFeedback", 0.4f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    // CHORUS PRESETS
    if (presetName == "Lush Chorus") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.5f);
        setParameterValue("chorusRate", 0.8f);
        setParameterValue("chorusDepth", 0.6f);
        setParameterValue("chorusCenterDelay", 18.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Subtle Shimmer") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.35f);
        setParameterValue("chorusRate", 0.4f);
        setParameterValue("chorusDepth", 0.4f);
        setParameterValue("chorusCenterDelay", 12.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Wide Stereo") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.6f);
        setParameterValue("chorusRate", 2.5f);
        setParameterValue("chorusDepth", 0.7f);
        setParameterValue("chorusCenterDelay", 20.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Studio Thickener") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.3f);
        setParameterValue("chorusRate", 1.5f);
        setParameterValue("chorusDepth", 0.55f);
        setParameterValue("chorusCenterDelay", 14.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    // COMBO PRESETS
    if (presetName == "Ambient Space") {
        setParameterValue("reverbMix", 0.35f);
        setParameterValue("reverbRoomSize", 0.7f);
        setParameterValue("reverbDamping", 0.4f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.25f);
        setParameterValue("delayTime", 450.0f);
        setParameterValue("delayFeedback", 0.55f);
        setParameterValue("chorusMix", 0.2f);
        setParameterValue("chorusRate", 0.8f);
        setParameterValue("chorusDepth", 0.5f);
        setParameterValue("chorusCenterDelay", 15.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Dream Machine") {
        setParameterValue("reverbMix", 0.5f);
        setParameterValue("reverbRoomSize", 0.9f);
        setParameterValue("reverbDamping", 0.2f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.3f);
        setParameterValue("delayTime", 600.0f);
        setParameterValue("delayFeedback", 0.6f);
        setParameterValue("chorusMix", 0.25f);
        setParameterValue("chorusRate", 0.6f);
        setParameterValue("chorusDepth", 0.7f);
        setParameterValue("chorusCenterDelay", 20.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Infinite Void") {
        setParameterValue("reverbMix", 0.55f);
        setParameterValue("reverbRoomSize", 0.95f);
        setParameterValue("reverbDamping", 0.15f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.15f);
        setParameterValue("delayTime", 800.0f);
        setParameterValue("delayFeedback", 0.8f);
        setParameterValue("chorusMix", 0.3f);
        setParameterValue("chorusRate", 0.3f);
        setParameterValue("chorusDepth", 0.8f);
        setParameterValue("chorusCenterDelay", 22.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    // NEW REVERB PRESETS
    if (presetName == "Dark Chamber") {
        setParameterValue("reverbMix", 0.4f);
        setParameterValue("reverbRoomSize", 0.65f);
        setParameterValue("reverbDamping", 0.85f);
        setParameterValue("reverbWidth", 0.9f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.2f);
        setParameterValue("filterCutoff", 1200.0f);
        setParameterValue("filterResonance", 0.4f);
        setParameterValue("filterDrive", 1.5f);
        setParameterValue("filterType", 0); // Low Pass
        return;
    }
    
    if (presetName == "Spring Tank") {
        setParameterValue("reverbMix", 0.5f);
        setParameterValue("reverbRoomSize", 0.4f);
        setParameterValue("reverbDamping", 0.3f);
        setParameterValue("reverbWidth", 0.7f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.15f);
        setParameterValue("filterCutoff", 3500.0f);
        setParameterValue("filterResonance", 0.6f);
        setParameterValue("filterDrive", 2.0f);
        setParameterValue("filterType", 2); // Band Pass
        return;
    }
    
    // NEW DELAY PRESETS
    if (presetName == "Triplet Delay") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.42f);
        setParameterValue("delayTime", 333.0f); // Triplet timing
        setParameterValue("delayFeedback", 0.45f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Long Tail") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.38f);
        setParameterValue("delayTime", 1200.0f);
        setParameterValue("delayFeedback", 0.75f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.18f);
        setParameterValue("filterCutoff", 2500.0f);
        setParameterValue("filterResonance", 0.3f);
        setParameterValue("filterDrive", 1.2f);
        setParameterValue("filterType", 0); // Low Pass
        return;
    }
    
    // NEW CHORUS PRESETS
    if (presetName == "Vibrato") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.8f);
        setParameterValue("chorusRate", 5.0f);
        setParameterValue("chorusDepth", 0.9f);
        setParameterValue("chorusCenterDelay", 10.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Ensemble") {
        setParameterValue("reverbMix", 0.0f);
        setParameterValue("delayMix", 0.0f);
        setParameterValue("chorusMix", 0.45f);
        setParameterValue("chorusRate", 0.6f);
        setParameterValue("chorusDepth", 0.5f);
        setParameterValue("chorusCenterDelay", 25.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    // NEW COMBO PRESETS
    if (presetName == "Vocal Air") {
        setParameterValue("reverbMix", 0.25f);
        setParameterValue("reverbRoomSize", 0.45f);
        setParameterValue("reverbDamping", 0.55f);
        setParameterValue("reverbWidth", 0.9f);
        setParameterValue("delayMix", 0.15f);
        setParameterValue("delayTime", 180.0f);
        setParameterValue("delayFeedback", 0.25f);
        setParameterValue("chorusMix", 0.12f);
        setParameterValue("chorusRate", 0.5f);
        setParameterValue("chorusDepth", 0.35f);
        setParameterValue("chorusCenterDelay", 12.0f);
        setParameterValue("filterMix", 0.12f);
        setParameterValue("filterCutoff", 8000.0f);
        setParameterValue("filterResonance", 0.25f);
        setParameterValue("filterDrive", 1.0f);
        setParameterValue("filterType", 1); // High Pass
        return;
    }
    
    if (presetName == "Synth Pad") {
        setParameterValue("reverbMix", 0.45f);
        setParameterValue("reverbRoomSize", 0.85f);
        setParameterValue("reverbDamping", 0.35f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.2f);
        setParameterValue("delayTime", 500.0f);
        setParameterValue("delayFeedback", 0.5f);
        setParameterValue("chorusMix", 0.35f);
        setParameterValue("chorusRate", 1.2f);
        setParameterValue("chorusDepth", 0.65f);
        setParameterValue("chorusCenterDelay", 18.0f);
        setParameterValue("filterMix", 0.25f);
        setParameterValue("filterCutoff", 3000.0f);
        setParameterValue("filterResonance", 0.5f);
        setParameterValue("filterDrive", 1.0f);
        setParameterValue("filterType", 0); // Low Pass
        return;
    }
    
    if (presetName == "Guitar Shimmer") {
        setParameterValue("reverbMix", 0.35f);
        setParameterValue("reverbRoomSize", 0.75f);
        setParameterValue("reverbDamping", 0.25f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.28f);
        setParameterValue("delayTime", 400.0f);
        setParameterValue("delayFeedback", 0.48f);
        setParameterValue("chorusMix", 0.22f);
        setParameterValue("chorusRate", 1.8f);
        setParameterValue("chorusDepth", 0.55f);
        setParameterValue("chorusCenterDelay", 16.0f);
        setParameterValue("filterMix", 0.0f);
        return;
    }
    
    if (presetName == "Lo-Fi Vibes") {
        setParameterValue("reverbMix", 0.3f);
        setParameterValue("reverbRoomSize", 0.35f);
        setParameterValue("reverbDamping", 0.8f);
        setParameterValue("reverbWidth", 0.6f);
        setParameterValue("delayMix", 0.35f);
        setParameterValue("delayTime", 350.0f);
        setParameterValue("delayFeedback", 0.6f);
        setParameterValue("chorusMix", 0.2f);
        setParameterValue("chorusRate", 0.4f);
        setParameterValue("chorusDepth", 0.7f);
        setParameterValue("chorusCenterDelay", 20.0f);
        setParameterValue("filterMix", 0.4f);
        setParameterValue("filterCutoff", 1800.0f);
        setParameterValue("filterResonance", 0.45f);
        setParameterValue("filterDrive", 2.5f);
        setParameterValue("filterType", 0); // Low Pass
        return;
    }
    
    if (presetName == "Psychedelic") {
        setParameterValue("reverbMix", 0.5f);
        setParameterValue("reverbRoomSize", 0.9f);
        setParameterValue("reverbDamping", 0.2f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.4f);
        setParameterValue("delayTime", 666.0f);
        setParameterValue("delayFeedback", 0.7f);
        setParameterValue("chorusMix", 0.4f);
        setParameterValue("chorusRate", 3.5f);
        setParameterValue("chorusDepth", 0.85f);
        setParameterValue("chorusCenterDelay", 22.0f);
        setParameterValue("filterMix", 0.3f);
        setParameterValue("filterCutoff", 2200.0f);
        setParameterValue("filterResonance", 0.7f);
        setParameterValue("filterDrive", 2.0f);
        setParameterValue("filterType", 2); // Band Pass
        return;
    }
    
    if (presetName == "Epic Drums") {
        setParameterValue("reverbMix", 0.38f);
        setParameterValue("reverbRoomSize", 0.8f);
        setParameterValue("reverbDamping", 0.5f);
        setParameterValue("reverbWidth", 1.0f);
        setParameterValue("delayMix", 0.18f);
        setParameterValue("delayTime", 250.0f);
        setParameterValue("delayFeedback", 0.35f);
        setParameterValue("chorusMix", 0.0f);
        setParameterValue("filterMix", 0.22f);
        setParameterValue("filterCutoff", 4500.0f);
        setParameterValue("filterResonance", 0.35f);
        setParameterValue("filterDrive", 1.8f);
        setParameterValue("filterType", 1); // High Pass
        return;
    }
}

BombFXAudioProcessorEditor::~BombFXAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void BombFXAudioProcessorEditor::paint(juce::Graphics& g) {
    // Slick dark gradient background
    auto bounds = getLocalBounds();
    juce::ColourGradient gradient(
        juce::Colour(0xff1a1f3a), 0, 0,
        juce::Colour(0xff0d1428), bounds.getWidth(), bounds.getHeight(),
        false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);
    
    // Subtle noise texture overlay
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    juce::Random& random = juce::Random::getSystemRandom();
    for (int i = 0; i < 400; i++) {
        auto x = random.nextInt(bounds.getWidth());
        auto y = random.nextInt(bounds.getHeight());
        g.fillRect(x, y, 1, 1);
    }
    
    // Draw section backgrounds with subtle glow
    auto drawSectionBg = [&](int x, int width) {
        juce::Rectangle<float> rect(x + 10, 100, width - 20, getHeight() - 120);
        g.setColour(juce::Colour(0xff1e2442).withAlpha(0.5f));
        g.fillRoundedRectangle(rect, 8.0f);
        
        // Inner glow
        g.setColour(juce::Colour(0xff8b5cf6).withAlpha(0.05f));
        g.drawRoundedRectangle(rect.reduced(1), 8.0f, 1.5f);
    };
    
    drawSectionBg(0, 275);
    drawSectionBg(275, 255);
    drawSectionBg(530, 255);
    drawSectionBg(785, 315);
    
    // Draw logo in top left with glow
    if (logoImage.isValid()) {
        auto logoBounds = juce::Rectangle<int>(15, 10, 50, 50);
        
        // Glow effect
        g.setColour(juce::Colour(0xff8b5cf6).withAlpha(0.3f));
        g.fillEllipse(logoBounds.toFloat().expanded(5));
        
        g.drawImage(logoImage, logoBounds.toFloat(), juce::RectanglePlacement::centred);
    }
    
    // Title with enhanced styling
    auto titleBounds = getLocalBounds().removeFromTop(70).reduced(20, 10).withTrimmedLeft(70);
    
    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.setFont(juce::Font(42.0f, juce::Font::bold));
    g.drawText("BOMB", titleBounds.translated(2, 2), juce::Justification::centredLeft);
    
    // Purple glow
    g.setColour(juce::Colour(0xff8b5cf6).withAlpha(0.6f));
    g.drawText("BOMB", titleBounds.translated(1, 1), juce::Justification::centredLeft);
    
    // Main text
    g.setColour(juce::Colour(0xff8b5cf6));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("BOMB", titleBounds, juce::Justification::centredLeft);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("FX", titleBounds.withTrimmedLeft(120), juce::Justification::centredLeft);
    
    // Elegant section dividers with gradient
    auto drawDivider = [&](float x) {
        juce::ColourGradient divGrad(
            juce::Colour(0xff00d4ff).withAlpha(0.0f), x, 110,
            juce::Colour(0xff00d4ff).withAlpha(0.4f), x, getHeight() / 2,
            false);
        divGrad.addColour(1.0, juce::Colour(0xff00d4ff).withAlpha(0.0f));
        g.setGradientFill(divGrad);
        g.fillRect(x - 1, 110.0f, 2.0f, static_cast<float>(getHeight() - 130));
    };
    
    drawDivider(275);
    drawDivider(530);
    drawDivider(785);
    
    // Bottom accent line
    g.setColour(juce::Colour(0xff8b5cf6).withAlpha(0.3f));
    g.fillRect(20, getHeight() - 5, getWidth() - 40, 2);
}

void BombFXAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    
    // Top bar for preset selector - aligned to top right
    auto topBar = bounds.removeFromTop(70).reduced(20, 15);
    
    // Buttons on far right
    auto buttonArea = topBar.removeFromRight(150);
    savePresetButton.setBounds(buttonArea.removeFromLeft(70).withHeight(30));
    buttonArea.removeFromLeft(5);
    loadUserPresetsButton.setBounds(buttonArea.withHeight(30));
    
    topBar.removeFromLeft(10); // Spacing
    auto rightSide = topBar.removeFromRight(280);
    
    presetLabel.setBounds(rightSide.removeFromLeft(70).withHeight(30));
    rightSide.removeFromLeft(10);
    presetSelector.setBounds(rightSide.withHeight(30));
    
    bounds.removeFromTop(40); // Spacing
    bounds.reduce(20, 20);
    
    auto sectionWidth = bounds.getWidth() / 4; // Now 4 sections
    
    // === REVERB ===
    auto reverbArea = bounds.removeFromLeft(sectionWidth);
    reverbLabel.setBounds(reverbArea.removeFromTop(30));
    reverbArea.removeFromTop(10);
    
    auto reverbContent = reverbArea.reduced(10);
    auto reverbLeft = reverbContent.removeFromLeft(60); // Fader area
    
    reverbMixLabel.setBounds(reverbLeft.removeFromTop(15));
    reverbLeft.removeFromTop(5);
    reverbMixSlider.setBounds(reverbLeft.removeFromTop(280).reduced(15, 0));
    
    // Knobs on the right
    auto reverbKnobs = reverbContent.removeFromLeft(180);
    int knobSize = 70;
    
    auto reverbRow1 = reverbKnobs.removeFromTop(knobSize + 45); // Extra space for labels
    auto roomArea = reverbRow1.removeFromLeft(knobSize + 10);
    reverbRoomSizeLabel.setBounds(roomArea.removeFromTop(15));
    reverbRoomSizeSlider.setBounds(roomArea.withSizeKeepingCentre(knobSize, knobSize));
    
    reverbDampingLabel.setBounds(reverbRow1.removeFromTop(15));
    reverbDampingSlider.setBounds(reverbRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    reverbKnobs.removeFromTop(10);
    auto widthArea = reverbKnobs.removeFromTop(knobSize + 45);
    reverbWidthLabel.setBounds(widthArea.removeFromTop(15));
    reverbWidthSlider.setBounds(widthArea.withSizeKeepingCentre(knobSize, knobSize));
    
    // === DELAY ===
    auto delayArea = bounds.removeFromLeft(sectionWidth);
    delayLabel.setBounds(delayArea.removeFromTop(30));
    delayArea.removeFromTop(10);
    
    auto delayContent = delayArea.reduced(10);
    auto delayLeft = delayContent.removeFromLeft(60);
    
    delayMixLabel.setBounds(delayLeft.removeFromTop(15));
    delayLeft.removeFromTop(5);
    delayMixSlider.setBounds(delayLeft.removeFromTop(280).reduced(15, 0));
    
    auto delayKnobs = delayContent.removeFromLeft(180);
    
    auto delayRow1 = delayKnobs.removeFromTop(knobSize + 45);
    auto timeArea = delayRow1.removeFromLeft(knobSize + 10);
    delayTimeLabel.setBounds(timeArea.removeFromTop(15));
    delayTimeSlider.setBounds(timeArea.withSizeKeepingCentre(knobSize, knobSize));
    
    delayFeedbackLabel.setBounds(delayRow1.removeFromTop(15));
    delayFeedbackSlider.setBounds(delayRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    // === CHORUS ===
    auto chorusArea = bounds.removeFromLeft(sectionWidth);
    chorusLabel.setBounds(chorusArea.removeFromTop(30));
    chorusArea.removeFromTop(10);
    
    auto chorusContent = chorusArea.reduced(10);
    auto chorusLeft = chorusContent.removeFromLeft(60);
    
    chorusMixLabel.setBounds(chorusLeft.removeFromTop(15));
    chorusLeft.removeFromTop(5);
    chorusMixSlider.setBounds(chorusLeft.removeFromTop(280).reduced(15, 0));
    
    auto chorusKnobs = chorusContent.removeFromLeft(180);
    
    auto chorusRow1 = chorusKnobs.removeFromTop(knobSize + 45);
    auto rateArea = chorusRow1.removeFromLeft(knobSize + 10);
    chorusRateLabel.setBounds(rateArea.removeFromTop(15));
    chorusRateSlider.setBounds(rateArea.withSizeKeepingCentre(knobSize, knobSize));
    
    chorusDepthLabel.setBounds(chorusRow1.removeFromTop(15));
    chorusDepthSlider.setBounds(chorusRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    chorusKnobs.removeFromTop(10);
    auto centerDelayArea = chorusKnobs.removeFromTop(knobSize + 45);
    chorusCenterDelayLabel.setBounds(centerDelayArea.removeFromTop(15));
    chorusCenterDelaySlider.setBounds(centerDelayArea.withSizeKeepingCentre(knobSize, knobSize));
    
    // === FILTER ===
    auto filterArea = bounds;
    filterLabel.setBounds(filterArea.removeFromTop(30));
    filterArea.removeFromTop(10);
    
    auto filterContent = filterArea.reduced(10);
    auto filterLeft = filterContent.removeFromLeft(60);
    
    filterMixLabel.setBounds(filterLeft.removeFromTop(15));
    filterLeft.removeFromTop(5);
    filterMixSlider.setBounds(filterLeft.removeFromTop(280).reduced(15, 0));
    
    auto filterKnobs = filterContent.removeFromLeft(180);
    
    // Filter type selector at top
    auto typeArea = filterKnobs.removeFromTop(45);
    filterTypeLabel.setBounds(typeArea.removeFromTop(15));
    filterTypeSelector.setBounds(typeArea.reduced(5, 0));
    filterKnobs.removeFromTop(10);
    
    auto filterRow1 = filterKnobs.removeFromTop(knobSize + 45);
    auto cutoffArea = filterRow1.removeFromLeft(knobSize + 10);
    filterCutoffLabel.setBounds(cutoffArea.removeFromTop(15));
    filterCutoffSlider.setBounds(cutoffArea.withSizeKeepingCentre(knobSize, knobSize));
    
    filterResonanceLabel.setBounds(filterRow1.removeFromTop(15));
    filterResonanceSlider.setBounds(filterRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    filterKnobs.removeFromTop(10);
    auto driveArea = filterKnobs.removeFromTop(knobSize + 45);
    filterDriveLabel.setBounds(driveArea.removeFromTop(15));
    filterDriveSlider.setBounds(driveArea.withSizeKeepingCentre(knobSize, knobSize));
}

juce::File BombFXAudioProcessorEditor::getUserPresetsDirectory() {
    auto docs = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    auto presetDir = docs.getChildFile("BombFX").getChildFile("Presets");
    if (!presetDir.exists())
        presetDir.createDirectory();
    return presetDir;
}

void BombFXAudioProcessorEditor::saveCurrentPreset() {
    // Create and show alert window asynchronously
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withTitle("Save Preset")
            .withMessage("Enter preset name:")
            .withButton("Save")
            .withButton("Cancel")
            .withIconType(juce::MessageBoxIconType::NoIcon)
            .withAssociatedComponent(this),
        [this](int result) {
            if (result == 0) return; // Cancelled
            
            // For now, use a default name based on timestamp
            auto presetName = "My Preset " + juce::Time::getCurrentTime().formatted("%Y%m%d_%H%M%S");
            
            // Get current parameter values
            juce::XmlElement preset("BombFXPreset");
            preset.setAttribute("name", presetName);
            
            // Save all float parameters
            auto& apvts = audioProcessor.getParameters();
            
            if (auto* p = apvts.getParameter("reverbMix"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("reverbMix", fp->get());
            
            if (auto* p = apvts.getParameter("reverbRoomSize"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("reverbRoomSize", fp->get());
            
            if (auto* p = apvts.getParameter("reverbDamping"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("reverbDamping", fp->get());
            
            if (auto* p = apvts.getParameter("reverbWidth"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("reverbWidth", fp->get());
            
            if (auto* p = apvts.getParameter("delayMix"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("delayMix", fp->get());
            
            if (auto* p = apvts.getParameter("delayTime"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("delayTime", fp->get());
            
            if (auto* p = apvts.getParameter("delayFeedback"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("delayFeedback", fp->get());
            
            if (auto* p = apvts.getParameter("chorusMix"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("chorusMix", fp->get());
            
            if (auto* p = apvts.getParameter("chorusRate"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("chorusRate", fp->get());
            
            if (auto* p = apvts.getParameter("chorusDepth"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("chorusDepth", fp->get());
            
            if (auto* p = apvts.getParameter("chorusCenterDelay"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("chorusCenterDelay", fp->get());
            
            if (auto* p = apvts.getParameter("filterMix"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("filterMix", fp->get());
            
            if (auto* p = apvts.getParameter("filterCutoff"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("filterCutoff", fp->get());
            
            if (auto* p = apvts.getParameter("filterResonance"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("filterResonance", fp->get());
            
            if (auto* p = apvts.getParameter("filterDrive"))
                if (auto* fp = dynamic_cast<juce::AudioParameterFloat*>(p))
                    preset.setAttribute("filterDrive", fp->get());
            
            if (auto* p = apvts.getParameter("filterType"))
                if (auto* cp = dynamic_cast<juce::AudioParameterChoice*>(p))
                    preset.setAttribute("filterType", cp->getIndex());
            
            // Save to file
            auto presetFile = getUserPresetsDirectory().getChildFile(presetName + ".bombfx");
            if (preset.writeTo(presetFile)) {
                juce::AlertWindow::showAsync(
                    juce::MessageBoxOptions()
                        .withTitle("Success")
                        .withMessage("Preset '" + presetName + "' saved!")
                        .withButton("OK")
                        .withIconType(juce::MessageBoxIconType::InfoIcon),
                    nullptr);
                
                // Refresh the preset list
                loadUserPresets();
            }
        });
}

void BombFXAudioProcessorEditor::loadUserPresets() {
    // Clear and rebuild preset menu with user presets
    presetSelector.clear();
    populatePresets(); // Add factory presets
    
    // Find user preset files
    auto presetDir = getUserPresetsDirectory();
    auto presetFiles = presetDir.findChildFiles(juce::File::findFiles, false, "*.bombfx");
    
    if (presetFiles.size() > 0) {
        presetSelector.addSeparator();
        presetSelector.addItem("--- USER PRESETS ---", 500);
        
        // Add user presets
        int id = 501;
        for (const auto& file : presetFiles) {
            presetSelector.addItem(file.getFileNameWithoutExtension(), id++);
        }
    }
}
