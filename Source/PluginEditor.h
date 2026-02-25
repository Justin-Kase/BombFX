#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "BombFXLookAndFeel.h"

class BombFXAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    BombFXAudioProcessorEditor(BombFXAudioProcessor&);
    ~BombFXAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BombFXAudioProcessor& audioProcessor;
    BombFXLookAndFeel customLookAndFeel;
    
    // Preset selector
    juce::ComboBox presetSelector;
    juce::Label presetLabel;
    void loadPreset(const juce::String& presetName);
    void populatePresets();
    
    // Reverb controls
    juce::Slider reverbMixSlider;
    juce::Slider reverbRoomSizeSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbWidthSlider;
    juce::Label reverbLabel;
    juce::Label reverbMixLabel;
    
    // Delay controls
    juce::Slider delayMixSlider;
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Label delayLabel;
    juce::Label delayMixLabel;
    
    // Chorus controls
    juce::Slider chorusMixSlider;
    juce::Slider chorusRateSlider;
    juce::Slider chorusDepthSlider;
    juce::Slider chorusCenterDelaySlider;
    juce::Label chorusLabel;
    juce::Label chorusMixLabel;
    
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BombFXAudioProcessorEditor)
};
