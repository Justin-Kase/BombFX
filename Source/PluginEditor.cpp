#include "PluginEditor.h"

BombFXAudioProcessorEditor::BombFXAudioProcessorEditor(BombFXAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    
    setSize(800, 500);
    
    auto& params = audioProcessor.getParameters();
    
    // === REVERB ===
    reverbLabel.setText("REVERB", juce::dontSendNotification);
    reverbLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    reverbLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(reverbLabel);
    
    auto setupSlider = [this](juce::Slider& slider, const juce::String& paramID) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        addAndMakeVisible(slider);
        sliderAttachments.push_back(
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.getParameters(), paramID, slider));
    };
    
    setupSlider(reverbMixSlider, "reverbMix");
    setupSlider(reverbRoomSizeSlider, "reverbRoomSize");
    setupSlider(reverbDampingSlider, "reverbDamping");
    setupSlider(reverbWidthSlider, "reverbWidth");
    
    // === DELAY ===
    delayLabel.setText("DELAY", juce::dontSendNotification);
    delayLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    delayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(delayLabel);
    
    setupSlider(delayMixSlider, "delayMix");
    setupSlider(delayTimeSlider, "delayTime");
    setupSlider(delayFeedbackSlider, "delayFeedback");
    
    // === CHORUS ===
    chorusLabel.setText("CHORUS", juce::dontSendNotification);
    chorusLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    chorusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(chorusLabel);
    
    setupSlider(chorusMixSlider, "chorusMix");
    setupSlider(chorusRateSlider, "chorusRate");
    setupSlider(chorusDepthSlider, "chorusDepth");
    setupSlider(chorusCenterDelaySlider, "chorusCenterDelay");
}

BombFXAudioProcessorEditor::~BombFXAudioProcessorEditor() {}

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
    
    // Title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(32.0f, juce::Font::bold));
    g.drawText("💣 BOMB FX", bounds.removeFromTop(60), juce::Justification::centred);
    
    // Section dividers
    g.setColour(juce::Colour(0xff00d4ff).withAlpha(0.3f));
    g.drawLine(260, 80, 260, getHeight() - 20, 2.0f);
    g.drawLine(540, 80, 540, getHeight() - 20, 2.0f);
}

void BombFXAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(80); // Title space
    bounds.reduce(20, 20);
    
    auto sectionWidth = bounds.getWidth() / 3;
    
    // === REVERB ===
    auto reverbArea = bounds.removeFromLeft(sectionWidth);
    reverbLabel.setBounds(reverbArea.removeFromTop(40));
    
    auto reverbKnobArea = reverbArea.reduced(10);
    int knobSize = 100;
    int knobSpacing = 10;
    
    reverbMixSlider.setBounds(reverbKnobArea.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
    reverbKnobArea.removeFromTop(knobSpacing);
    
    auto reverbRow = reverbKnobArea.removeFromTop(knobSize + 30);
    reverbRoomSizeSlider.setBounds(reverbRow.removeFromLeft(knobSize + 20).withSizeKeepingCentre(knobSize, knobSize));
    reverbDampingSlider.setBounds(reverbRow.removeFromLeft(knobSize + 20).withSizeKeepingCentre(knobSize, knobSize));
    
    reverbKnobArea.removeFromTop(knobSpacing);
    reverbWidthSlider.setBounds(reverbKnobArea.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
    
    // === DELAY ===
    auto delayArea = bounds.removeFromLeft(sectionWidth);
    delayLabel.setBounds(delayArea.removeFromTop(40));
    
    auto delayKnobArea = delayArea.reduced(10);
    delayMixSlider.setBounds(delayKnobArea.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
    delayKnobArea.removeFromTop(knobSpacing);
    
    auto delayRow = delayKnobArea.removeFromTop(knobSize + 30);
    delayTimeSlider.setBounds(delayRow.removeFromLeft(knobSize + 20).withSizeKeepingCentre(knobSize, knobSize));
    delayFeedbackSlider.setBounds(delayRow.withSizeKeepingCentre(knobSize, knobSize));
    
    // === CHORUS ===
    auto chorusArea = bounds;
    chorusLabel.setBounds(chorusArea.removeFromTop(40));
    
    auto chorusKnobArea = chorusArea.reduced(10);
    chorusMixSlider.setBounds(chorusKnobArea.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
    chorusKnobArea.removeFromTop(knobSpacing);
    
    auto chorusRow = chorusKnobArea.removeFromTop(knobSize + 30);
    chorusRateSlider.setBounds(chorusRow.removeFromLeft(knobSize + 20).withSizeKeepingCentre(knobSize, knobSize));
    chorusDepthSlider.setBounds(chorusRow.withSizeKeepingCentre(knobSize, knobSize));
    
    chorusKnobArea.removeFromTop(knobSpacing);
    chorusCenterDelaySlider.setBounds(chorusKnobArea.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize));
}
