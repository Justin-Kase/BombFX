#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Reverb
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverbMix", "Reverb Mix", 0.0f, 1.0f, 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverbRoomSize", "Room Size", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverbDamping", "Damping", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("reverbWidth", "Width", 0.0f, 1.0f, 1.0f));
    
    // Delay
    layout.add(std::make_unique<juce::AudioParameterFloat>("delayMix", "Delay Mix", 0.0f, 1.0f, 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 1.0f, 2000.0f, 250.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("delayFeedback", "Feedback", 0.0f, 0.95f, 0.4f));
    
    // Chorus
    layout.add(std::make_unique<juce::AudioParameterFloat>("chorusMix", "Chorus Mix", 0.0f, 1.0f, 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("chorusRate", "Chorus Rate", 0.1f, 10.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("chorusDepth", "Chorus Depth", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("chorusCenterDelay", "Chorus Delay", 7.0f, 30.0f, 15.0f));
    
    // Filter
    layout.add(std::make_unique<juce::AudioParameterFloat>("filterMix", "Filter Mix", 0.0f, 1.0f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Cutoff", 20.0f, 20000.0f, 2000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("filterResonance", "Resonance", 0.0f, 1.0f, 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("filterDrive", "Drive", 1.0f, 10.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("filterType", "Filter Type", 
        juce::StringArray{"Low Pass", "High Pass", "Band Pass"}, 0));
    
    return layout;
}

BombFXAudioProcessor::BombFXAudioProcessor()
    : AudioProcessor(BusesProperties()
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout()) {
}

BombFXAudioProcessor::~BombFXAudioProcessor() {}

const juce::String BombFXAudioProcessor::getName() const { return JucePlugin_Name; }
bool BombFXAudioProcessor::acceptsMidi() const { return false; }
bool BombFXAudioProcessor::producesMidi() const { return false; }
bool BombFXAudioProcessor::isMidiEffect() const { return false; }
double BombFXAudioProcessor::getTailLengthSeconds() const { return 2.0; }
int BombFXAudioProcessor::getNumPrograms() { return 1; }
int BombFXAudioProcessor::getCurrentProgram() { return 0; }
void BombFXAudioProcessor::setCurrentProgram(int) {}
const juce::String BombFXAudioProcessor::getProgramName(int) { return {}; }
void BombFXAudioProcessor::changeProgramName(int, const juce::String&) {}

void BombFXAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    
    reverbProcessor.prepare(spec);
    delayProcessor.prepare(spec);
    chorusProcessor.prepare(spec);
    filterProcessor.prepare(spec);
    
    reverbBuffer.setSize(spec.numChannels, samplesPerBlock);
    delayBuffer.setSize(spec.numChannels, samplesPerBlock);
    chorusBuffer.setSize(spec.numChannels, samplesPerBlock);
}

void BombFXAudioProcessor::releaseResources() {
    reverbProcessor.reset();
    delayProcessor.reset();
    chorusProcessor.reset();
    filterProcessor.reset();
}

void BombFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    // Update parameters
    float reverbMix = parameters.getRawParameterValue("reverbMix")->load();
    reverbProcessor.setRoomSize(parameters.getRawParameterValue("reverbRoomSize")->load());
    reverbProcessor.setDamping(parameters.getRawParameterValue("reverbDamping")->load());
    reverbProcessor.setWidth(parameters.getRawParameterValue("reverbWidth")->load());
    reverbProcessor.setWetLevel(reverbMix);
    reverbProcessor.setDryLevel(0.0f); // We'll mix manually
    
    float delayMix = parameters.getRawParameterValue("delayMix")->load();
    delayProcessor.setDelayTime(parameters.getRawParameterValue("delayTime")->load());
    delayProcessor.setFeedback(parameters.getRawParameterValue("delayFeedback")->load());
    delayProcessor.setWetLevel(delayMix);
    delayProcessor.setDryLevel(0.0f);
    
    float chorusMix = parameters.getRawParameterValue("chorusMix")->load();
    chorusProcessor.setRate(parameters.getRawParameterValue("chorusRate")->load());
    chorusProcessor.setDepth(parameters.getRawParameterValue("chorusDepth")->load());
    chorusProcessor.setCenterDelay(parameters.getRawParameterValue("chorusCenterDelay")->load());
    chorusProcessor.setMix(chorusMix);
    
    // Copy dry signal
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);
    
    // Process each effect in parallel
    reverbBuffer.makeCopyOf(buffer);
    delayBuffer.makeCopyOf(buffer);
    chorusBuffer.makeCopyOf(buffer);
    
    reverbProcessor.process(reverbBuffer);
    delayProcessor.process(delayBuffer);
    chorusProcessor.process(chorusBuffer);
    
    // Mix: blend all three FX + original
    buffer.clear();
    for (int ch = 0; ch < numChannels; ++ch) {
        buffer.addFrom(ch, 0, dryBuffer, ch, 0, numSamples, 1.0f - reverbMix - delayMix - chorusMix);
        buffer.addFrom(ch, 0, reverbBuffer, ch, 0, numSamples, reverbMix);
        buffer.addFrom(ch, 0, delayBuffer, ch, 0, numSamples, delayMix);
        buffer.addFrom(ch, 0, chorusBuffer, ch, 0, numSamples, chorusMix);
    }
    
    // Apply filter at the end of the chain
    float filterMix = parameters.getRawParameterValue("filterMix")->load();
    filterProcessor.setCutoff(parameters.getRawParameterValue("filterCutoff")->load());
    filterProcessor.setResonance(parameters.getRawParameterValue("filterResonance")->load());
    filterProcessor.setDrive(parameters.getRawParameterValue("filterDrive")->load());
    filterProcessor.setWetLevel(filterMix);
    filterProcessor.setDryLevel(1.0f - filterMix);
    
    int filterTypeIndex = static_cast<int>(parameters.getRawParameterValue("filterType")->load());
    filterProcessor.setFilterType(static_cast<FilterProcessor::FilterType>(filterTypeIndex));
    
    filterProcessor.process(buffer);
}

bool BombFXAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* BombFXAudioProcessor::createEditor() {
    return new BombFXAudioProcessorEditor(*this);
}

void BombFXAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BombFXAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new BombFXAudioProcessor();
}
