#include "PluginEditor.h"

BombFXAudioProcessorEditor::BombFXAudioProcessorEditor(BombFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setSize(1100, 550);
    
    // Load logo
    juce::File logoFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
        .getParentDirectory().getParentDirectory().getParentDirectory().getParentDirectory()
        .getChildFile("Resources").getChildFile("logo.png");
    
    if (logoFile.existsAsFile())
        logoImage = juce::ImageFileFormat::loadFrom(logoFile);
    
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
    presetSelector.addItem("--- REVERB ---", 100);
    presetSelector.addItem("Cathedral", 1);
    presetSelector.addItem("Small Room", 2);
    presetSelector.addItem("Vocal Plate", 3);
    presetSelector.addItem("Huge Hall", 4);
    
    presetSelector.addItem("--- DELAY ---", 200);
    presetSelector.addItem("Slapback", 5);
    presetSelector.addItem("Dub Echo", 6);
    presetSelector.addItem("Ping Pong", 7);
    presetSelector.addItem("Dotted Eighth", 8);
    
    presetSelector.addItem("--- CHORUS ---", 300);
    presetSelector.addItem("Lush Chorus", 9);
    presetSelector.addItem("Subtle Shimmer", 10);
    presetSelector.addItem("Wide Stereo", 11);
    presetSelector.addItem("Studio Thickener", 12);
    
    presetSelector.addItem("--- COMBO ---", 400);
    presetSelector.addItem("Ambient Space", 13);
    presetSelector.addItem("Dream Machine", 14);
    presetSelector.addItem("Infinite Void", 15);
}

void BombFXAudioProcessorEditor::setParameterValue(const juce::String& paramID, float value) {
    if (auto* param = audioProcessor.getParameters().getParameter(paramID))
        param->setValueNotifyingHost(value);
}

void BombFXAudioProcessorEditor::loadPreset(const juce::String& presetName) {
    // Map preset name to category/file
    juce::String category;
    juce::String filename = presetName;
    
    if (presetName == "Cathedral" || presetName == "Small Room" || 
        presetName == "Vocal Plate" || presetName == "Huge Hall")
        category = "Reverb";
    else if (presetName == "Slapback" || presetName == "Dub Echo" || 
             presetName == "Ping Pong" || presetName == "Dotted Eighth")
        category = "Delay";
    else if (presetName == "Lush Chorus" || presetName == "Subtle Shimmer" || 
             presetName == "Wide Stereo" || presetName == "Studio Thickener")
        category = "Chorus";
    else if (presetName == "Ambient Space" || presetName == "Dream Machine" || 
             presetName == "Infinite Void")
        category = "Combo";
    else
        return; // Header item clicked
    
    // Build preset file path
    auto execPath = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    juce::File presetFile = execPath.getParentDirectory()
        .getParentDirectory().getParentDirectory().getParentDirectory()
        .getChildFile("Presets").getChildFile(category).getChildFile(filename + ".preset");
    
    if (!presetFile.existsAsFile())
        return;
    
    auto xml = juce::parseXML(presetFile);
    if (!xml)
        return;
    
    // Read and set each parameter - this will update both audio processor and UI
    if (auto* elem = xml->getChildByName("reverbMix"))
        setParameterValue("reverbMix", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("reverbRoomSize"))
        setParameterValue("reverbRoomSize", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("reverbDamping"))
        setParameterValue("reverbDamping", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("reverbWidth"))
        setParameterValue("reverbWidth", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("delayMix"))
        setParameterValue("delayMix", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("delayTime"))
        setParameterValue("delayTime", elem->getAllSubText().getFloatValue() / 2000.0f); // Normalize to 0-1
    
    if (auto* elem = xml->getChildByName("delayFeedback"))
        setParameterValue("delayFeedback", elem->getAllSubText().getFloatValue() / 0.95f); // Normalize to 0-1
    
    if (auto* elem = xml->getChildByName("chorusMix"))
        setParameterValue("chorusMix", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("chorusRate"))
        setParameterValue("chorusRate", (elem->getAllSubText().getFloatValue() - 0.1f) / 9.9f); // Normalize to 0-1
    
    if (auto* elem = xml->getChildByName("chorusDepth"))
        setParameterValue("chorusDepth", elem->getAllSubText().getFloatValue());
    
    if (auto* elem = xml->getChildByName("chorusCenterDelay"))
        setParameterValue("chorusCenterDelay", (elem->getAllSubText().getFloatValue() - 7.0f) / 23.0f); // Normalize to 0-1
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
    
    // Draw logo in top left
    if (logoImage.isValid()) {
        auto logoBounds = getLocalBounds().removeFromTop(70).removeFromLeft(100).reduced(15);
        g.drawImage(logoImage, logoBounds.toFloat(), juce::RectanglePlacement::centred);
    }
    
    // Title with purple accent
    auto titleBounds = getLocalBounds().removeFromTop(70).reduced(20, 10).withTrimmedLeft(90);
    g.setColour(juce::Colour(0xff8b5cf6));
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("BOMB", titleBounds, juce::Justification::centredLeft);
    
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(40.0f, juce::Font::bold));
    g.drawText("FX", titleBounds.withTrimmedLeft(120), juce::Justification::centredLeft);
    
    // Section dividers
    g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
    g.drawLine(280, 110, 280, getHeight() - 20, 2.0f);
    g.drawLine(530, 110, 530, getHeight() - 20, 2.0f);
    g.drawLine(780, 110, 780, getHeight() - 20, 2.0f);
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
    
    auto delayKnobs = delayContent.removeFromLeft(180);
    
    auto delayRow1 = delayKnobs.removeFromTop(knobSize + 30);
    delayTimeSlider.setBounds(delayRow1.removeFromLeft(knobSize + 10).withSizeKeepingCentre(knobSize, knobSize));
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
    
    auto chorusRow1 = chorusKnobs.removeFromTop(knobSize + 30);
    chorusRateSlider.setBounds(chorusRow1.removeFromLeft(knobSize + 10).withSizeKeepingCentre(knobSize, knobSize));
    chorusDepthSlider.setBounds(chorusRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    chorusKnobs.removeFromTop(10);
    chorusCenterDelaySlider.setBounds(chorusKnobs.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
    
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
    filterTypeSelector.setBounds(filterKnobs.removeFromTop(30).reduced(5, 0));
    filterKnobs.removeFromTop(10);
    
    auto filterRow1 = filterKnobs.removeFromTop(knobSize + 30);
    filterCutoffSlider.setBounds(filterRow1.removeFromLeft(knobSize + 10).withSizeKeepingCentre(knobSize, knobSize));
    filterResonanceSlider.setBounds(filterRow1.withSizeKeepingCentre(knobSize, knobSize));
    
    filterKnobs.removeFromTop(10);
    filterDriveSlider.setBounds(filterKnobs.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
}
