#include "DelayProcessor.h"

DelayProcessor::DelayProcessor() {}

void DelayProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate = spec.sampleRate;
    delayLine.prepare(spec);
    delayLine.setMaximumDelayInSamples(static_cast<int>(spec.sampleRate * 2.0)); // 2 sec max
    reset();
}

void DelayProcessor::process(juce::AudioBuffer<float>& buffer) {
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    const int delaySamples = static_cast<int>((delayTimeMs / 1000.0) * sampleRate);
    
    delayLine.setDelay(static_cast<float>(delaySamples));
    
    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i) {
            float input = channelData[i];
            float delayed = delayLine.popSample(channel);
            
            // Feedback
            delayLine.pushSample(channel, input + delayed * feedback);
            
            // Mix
            channelData[i] = input * dryLevel + delayed * wetLevel;
        }
    }
}

void DelayProcessor::reset() {
    delayLine.reset();
}

void DelayProcessor::setDelayTime(float timeMs) {
    delayTimeMs = juce::jlimit(1.0f, 2000.0f, timeMs);
}

void DelayProcessor::setFeedback(float fb) {
    feedback = juce::jlimit(0.0f, 0.95f, fb);
}

void DelayProcessor::setWetLevel(float level) {
    wetLevel = juce::jlimit(0.0f, 1.0f, level);
}

void DelayProcessor::setDryLevel(float level) {
    dryLevel = juce::jlimit(0.0f, 1.0f, level);
}
