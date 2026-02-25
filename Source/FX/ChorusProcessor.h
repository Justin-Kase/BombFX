#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class ChorusProcessor {
public:
    ChorusProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    
    // Parameters
    void setRate(float rateHz);          // LFO rate 0.1 - 10 Hz
    void setDepth(float depth);          // 0.0 - 1.0
    void setMix(float mix);              // 0.0 - 1.0
    void setCenterDelay(float delayMs);  // 7-30ms typical
    
private:
    juce::dsp::Chorus<float> chorus;
    float rate = 1.0f;
    float depth = 0.5f;
    float mix = 0.5f;
    float centerDelay = 15.0f;
};
