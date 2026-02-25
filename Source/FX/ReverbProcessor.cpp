#include "ReverbProcessor.h"

ReverbProcessor::ReverbProcessor() {
    params.roomSize = 0.5f;
    params.damping = 0.5f;
    params.wetLevel = 0.33f;
    params.dryLevel = 0.4f;
    params.width = 1.0f;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);
}

void ReverbProcessor::prepare(const juce::dsp::ProcessSpec& spec) {
    reverb.prepare(spec);
}

void ReverbProcessor::process(juce::AudioBuffer<float>& buffer) {
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

void ReverbProcessor::reset() {
    reverb.reset();
}

void ReverbProcessor::setRoomSize(float size) {
    params.roomSize = juce::jlimit(0.0f, 1.0f, size);
    reverb.setParameters(params);
}

void ReverbProcessor::setDamping(float damping) {
    params.damping = juce::jlimit(0.0f, 1.0f, damping);
    reverb.setParameters(params);
}

void ReverbProcessor::setWidth(float width) {
    params.width = juce::jlimit(0.0f, 1.0f, width);
    reverb.setParameters(params);
}

void ReverbProcessor::setWetLevel(float level) {
    params.wetLevel = juce::jlimit(0.0f, 1.0f, level);
    reverb.setParameters(params);
}

void ReverbProcessor::setDryLevel(float level) {
    params.dryLevel = juce::jlimit(0.0f, 1.0f, level);
    reverb.setParameters(params);
}
