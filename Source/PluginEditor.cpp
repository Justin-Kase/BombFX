#include "PluginEditor.h"

BombFXAudioProcessorEditor::BombFXAudioProcessorEditor(BombFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setSize(900, 550);
    
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
    presetSelector.onChange = [this] {
        if (presetSelector.getSelectedId() > 0)
            loadPreset(presetSelector.getText());
    };
    
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
}

void BombFXAudioProcessorEditor::populatePresets() {
    presetSelector.addItem("Cathedral", 1);
    presetSelector.addItem("Small Room", 2);
    presetSelector.addItem("Slapback", 3);
    presetSelector.addItem("Dub Echo", 4);
    presetSelector.addItem("Lush Chorus", 5);
    presetSelector.addItem("Subtle Shimmer", 6);
    presetSelector.addItem("Ambient Space", 7);
    presetSelector.addItem("Dream Machine", 8);
    presetSelector.addItem("Psychedelic", 9);
    presetSelector.addItem("Vocal Plate", 10);
    presetSelector.addItem("Ping Pong Madness", 11);
    presetSelector.addItem("Infinite Void", 12);
    presetSelector.addItem("Studio Thickener", 13);
    presetSelector.addItem("Tight & Punchy", 14);
}

void BombFXAudioProcessorEditor::loadPreset(const juce::String& presetName) {
    juce::File presetFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory().getParentDirectory().getParentDirectory()
        .getChildFile("Presets").getChildFile(presetName + ".xml");
    
    if (!presetFile.existsAsFile())
        return;
    
    auto xml = juce::parseXML(presetFile);
    if (xml) {
        auto& params = audioProcessor.getParameters();
        auto state = juce::ValueTree::fromXml(*xml);
        if (state.isValid())
            params.replaceState(state);
    }
}

BombFXAudioProcessorEditor::~BombFXAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void BombFXAudioProcessorEditor::paint(juce::Graphics& g) {
    // Slick dark gradient background
    g.fillAll(juce::Colour(0xff1a1a2e));
    
    auto bounds = getLocalBounds();
    juce::ColourGradient gradient(
        juce::Colour(0xff16213e), 0, 0,
        juce::Colour(0xff0f3460), bounds.getWidth(), bounds.getHeight(),
        false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);
    
    // Title with purple accent
    g.setColour(juce::Colour(0xff8b5cf6));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("BOMB", bounds.removeFromTop(70).reduced(20, 10), juce::Justification::centredLeft);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    auto titleBounds = getLocalBounds().removeFromTop(70).reduced(20, 10);
    g.drawText("FX", titleBounds.withTrimmedLeft(120), juce::Justification::centredLeft);
    
    // Section dividers
    g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
    g.drawLine(300, 110, 300, getHeight() - 20, 2.0f);
    g.drawLine(600, 110, 600, getHeight() - 20, 2.0f);
}

void BombFXAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    
    // Top bar for preset selector
    auto topBar = bounds.removeFromTop(70).reduced(20, 15);
    topBar.removeFromLeft(200); // Skip title area
    
    presetLabel.setBounds(topBar.removeFromLeft(70));
    topBar.removeFromLeft(10);
    presetSelector.setBounds(topBar.removeFromLeft(200).withHeight(30));
    
    bounds.removeFromTop(40); // Spacing
    bounds.reduce(20, 20);
    
    auto sectionWidth = bounds.getWidth() / 3;
    
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
    auto reverbKnobs = reverbContent.removeFromLeft(200);
    int knobSize = 80;
    
    auto reverbRow1 = reverbKnobs.removeFromTop(knobSize + 30);
    reverbRoomSizeSlider.setBounds(reverbRow1.removeFromLeft(knobSize + 10).withSizeKeepingCentre(knobSize, knobSize));
    reverbDampingSlider.setBounds(reverbRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    reverbKnobs.removeFromTop(10);
    reverbWidthSlider.setBounds(reverbKnobs.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
    
    // === DELAY ===
    auto delayArea = bounds.removeFromLeft(sectionWidth);
    delayLabel.setBounds(delayArea.removeFromTop(30));
    delayArea.removeFromTop(10);
    
    auto delayContent = delayArea.reduced(10);
    auto delayLeft = delayContent.removeFromLeft(60);
    
    delayMixLabel.setBounds(delayLeft.removeFromTop(15));
    delayLeft.removeFromTop(5);
    delayMixSlider.setBounds(delayLeft.removeFromTop(280).reduced(15, 0));
    
    auto delayKnobs = delayContent.removeFromLeft(200);
    
    auto delayRow1 = delayKnobs.removeFromTop(knobSize + 30);
    delayTimeSlider.setBounds(delayRow1.removeFromLeft(knobSize + 10).withSizeKeepingCentre(knobSize, knobSize));
    delayFeedbackSlider.setBounds(delayRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    // === CHORUS ===
    auto chorusArea = bounds;
    chorusLabel.setBounds(chorusArea.removeFromTop(30));
    chorusArea.removeFromTop(10);
    
    auto chorusContent = chorusArea.reduced(10);
    auto chorusLeft = chorusContent.removeFromLeft(60);
    
    chorusMixLabel.setBounds(chorusLeft.removeFromTop(15));
    chorusLeft.removeFromTop(5);
    chorusMixSlider.setBounds(chorusLeft.removeFromTop(280).reduced(15, 0));
    
    auto chorusKnobs = chorusContent.removeFromLeft(200);
    
    auto chorusRow1 = chorusKnobs.removeFromTop(knobSize + 30);
    chorusRateSlider.setBounds(chorusRow1.removeFromLeft(knobSize + 10).withSizeKeepingCentre(knobSize, knobSize));
    chorusDepthSlider.setBounds(chorusRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    chorusKnobs.removeFromTop(10);
    chorusCenterDelaySlider.setBounds(chorusKnobs.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
}
