#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class ReverbProcessor {
public:
    ReverbProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    
    // Parameters
    void setRoomSize(float size);      // 0.0 - 1.0
    void setDamping(float damping);    // 0.0 - 1.0
    void setWidth(float width);        // 0.0 - 1.0
    void setWetLevel(float level);     // 0.0 - 1.0
    void setDryLevel(float level);     // 0.0 - 1.0
    
private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters params;
};
