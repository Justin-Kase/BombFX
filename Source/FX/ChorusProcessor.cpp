#include "ChorusProcessor.h"

ChorusProcessor::ChorusProcessor() {}

void ChorusProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    chorus.prepare(spec);
    chorus.setRate(rate);
    chorus.setDepth(depth);
    chorus.setMix(mix);
    chorus.setCentreDelay(centerDelay);
    chorus.setFeedback(0.0f);
}

void ChorusProcessor::process(juce::AudioBuffer<float>& buffer) {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    chorus.process(context);
}

void ChorusProcessor::reset() {
    chorus.reset();
}

void ChorusProcessor::setRate(float rateHz) {
    rate = juce::jlimit(0.1f, 10.0f, rateHz);
    chorus.setRate(rate);
}

void ChorusProcessor::setDepth(float d) {
    depth = juce::jlimit(0.0f, 1.0f, d);
    chorus.setDepth(depth);
}

void ChorusProcessor::setMix(float m) {
    mix = juce::jlimit(0.0f, 1.0f, m);
    chorus.setMix(mix);
}

void ChorusProcessor::setCenterDelay(float delayMs) {
    centerDelay = juce::jlimit(7.0f, 30.0f, delayMs);
    chorus.setCentreDelay(centerDelay);
}
