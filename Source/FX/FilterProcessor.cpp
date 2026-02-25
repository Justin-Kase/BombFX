#include "FilterProcessor.h"

FilterProcessor::FilterProcessor() {}

void FilterProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    filterLeft.prepare(spec);
    filterRight.prepare(spec);
    filterLeft.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filterRight.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    reset();
}

void FilterProcessor::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    // Store dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    // Apply drive (saturation)
    if (drive > 1.0f) {
        for (int ch = 0; ch < numChannels; ++ch) {
            auto* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i) {
                float sample = channelData[i] * drive;
                // Soft clipping
                channelData[i] = std::tanh(sample);
            }
        }
    }
    
    // Process filter
    auto* leftData = buffer.getWritePointer(0);
    auto* rightData = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;
    
    for (int i = 0; i < numSamples; ++i) {
        leftData[i] = filterLeft.processSample(0, leftData[i]);
        if (rightData)
            rightData[i] = filterRight.processSample(1, rightData[i]);
    }
    
    // Mix dry/wet
    for (int ch = 0; ch < numChannels; ++ch) {
        buffer.applyGain(ch, 0, numSamples, wetLevel);
        buffer.addFrom(ch, 0, dryBuffer, ch, 0, numSamples, dryLevel);
    }
}

void FilterProcessor::reset() {
    filterLeft.reset();
    filterRight.reset();
}

void FilterProcessor::setCutoff(float cutoffHz) {
    float clampedCutoff = juce::jlimit(20.0f, 20000.0f, cutoffHz);
    filterLeft.setCutoffFrequency(clampedCutoff);
    filterRight.setCutoffFrequency(clampedCutoff);
}

void FilterProcessor::setResonance(float resonance) {
    float q = juce::jmap(resonance, 0.0f, 1.0f, 0.7f, 20.0f);
    filterLeft.setResonance(q);
    filterRight.setResonance(q);
}

void FilterProcessor::setFilterType(FilterType type) {
    currentType = type;
    juce::dsp::StateVariableTPTFilterType jucetype;
    
    switch (type) {
        case LowPass:
            jucetype = juce::dsp::StateVariableTPTFilterType::lowpass;
            break;
        case HighPass:
            jucetype = juce::dsp::StateVariableTPTFilterType::highpass;
            break;
        case BandPass:
            jucetype = juce::dsp::StateVariableTPTFilterType::bandpass;
            break;
    }
    
    filterLeft.setType(jucetype);
    filterRight.setType(jucetype);
}

void FilterProcessor::setWetLevel(float level) {
    wetLevel = juce::jlimit(0.0f, 1.0f, level);
}

void FilterProcessor::setDryLevel(float level) {
    dryLevel = juce::jlimit(0.0f, 1.0f, level);
}

void FilterProcessor::setDrive(float d) {
    drive = juce::jlimit(1.0f, 10.0f, d);
}
