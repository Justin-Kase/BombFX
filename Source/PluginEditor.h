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
    juce::TextButton savePresetButton;
    juce::TextButton loadUserPresetsButton;
    void loadPreset(const juce::String& presetName);
    void populatePresets();
    void saveCurrentPreset();
    void loadUserPresets();
    juce::File getUserPresetsDirectory();
    
    // Reverb controls
    juce::Slider reverbMixSlider;
    juce::Slider reverbRoomSizeSlider;
    juce::Slider reverbDampingSlider;
    juce::Slider reverbWidthSlider;
    juce::Label reverbLabel;
    juce::Label reverbMixLabel;
    juce::Label reverbRoomSizeLabel;
    juce::Label reverbDampingLabel;
    juce::Label reverbWidthLabel;
    
    // Delay controls
    juce::Slider delayMixSlider;
    juce::Slider delayTimeSlider;
    juce::Slider delayFeedbackSlider;
    juce::Label delayLabel;
    juce::Label delayMixLabel;
    juce::Label delayTimeLabel;
    juce::Label delayFeedbackLabel;
    
    // Chorus controls
    juce::Slider chorusMixSlider;
    juce::Slider chorusRateSlider;
    juce::Slider chorusDepthSlider;
    juce::Slider chorusCenterDelaySlider;
    juce::Label chorusLabel;
    juce::Label chorusMixLabel;
    juce::Label chorusRateLabel;
    juce::Label chorusDepthLabel;
    juce::Label chorusCenterDelayLabel;
    
    // Filter controls
    juce::Slider filterMixSlider;
    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;
    juce::Slider filterDriveSlider;
    juce::ComboBox filterTypeSelector;
    juce::Label filterLabel;
    juce::Label filterMixLabel;
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;
    juce::Label filterDriveLabel;
    juce::Label filterTypeLabel;
    
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sliderAttachments;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    
    juce::Image logoImage;
    
    void setParameterValue(const juce::String& paramID, float value);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BombFXAudioProcessorEditor)
};
