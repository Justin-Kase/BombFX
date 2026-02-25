#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "FX/ReverbProcessor.h"
#include "FX/DelayProcessor.h"
#include "FX/ChorusProcessor.h"
#include "FX/FilterProcessor.h"

class BombFXAudioProcessor : public juce::AudioProcessor {
public:
    BombFXAudioProcessor();
    ~BombFXAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter access
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

private:
    juce::AudioProcessorValueTreeState parameters;
    
    ReverbProcessor reverbProcessor;
    DelayProcessor delayProcessor;
    ChorusProcessor chorusProcessor;
    FilterProcessor filterProcessor;
    
    // Temp buffers for parallel processing
    juce::AudioBuffer<float> reverbBuffer;
    juce::AudioBuffer<float> delayBuffer;
    juce::AudioBuffer<float> chorusBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BombFXAudioProcessor)
};
