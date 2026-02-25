#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class FilterProcessor {
public:
    enum FilterType {
        LowPass,
        HighPass,
        BandPass
    };
    
    FilterProcessor();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();
    
    // Parameters
    void setCutoff(float cutoffHz);        // 20 - 20000 Hz
    void setResonance(float resonance);    // 0.0 - 1.0
    void setFilterType(FilterType type);
    void setWetLevel(float level);         // 0.0 - 1.0
    void setDryLevel(float level);         // 0.0 - 1.0
    void setDrive(float drive);            // 1.0 - 10.0
    
private:
    using Filter = juce::dsp::StateVariableTPTFilter<float>;
    Filter filterLeft, filterRight;
    FilterType currentType = LowPass;
    float wetLevel = 0.5f;
    float dryLevel = 1.0f;
    float drive = 1.0f;
    double sampleRate = 44100.0;
};
