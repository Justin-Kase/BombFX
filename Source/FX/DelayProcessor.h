#pragma once
#include <JuceHeader.h>

class DelayProcessor {
public:
    DelayProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    
    // Parameters
    void setDelayTime(float timeMs);     // milliseconds
    void setFeedback(float feedback);    // 0.0 - 0.95
    void setWetLevel(float level);       // 0.0 - 1.0
    void setDryLevel(float level);       // 0.0 - 1.0
    
private:
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    float feedback = 0.4f;
    float wetLevel = 0.5f;
    float dryLevel = 1.0f;
    float delayTimeMs = 250.0f;
    double sampleRate = 44100.0;
};
