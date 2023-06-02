/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicCompAudioProcessor::BasicCompAudioProcessor()
                        #ifndef JucePlugin_PreferredChannelConfigurations
                            : AudioProcessor(BusesProperties()
                        #if ! JucePlugin_IsMidiEffect
                        #if ! JucePlugin_IsSynth
                                .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        #endif
                                .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                        #endif
                            )

                            , treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
                        #endif
{
    treeState.addParameterListener("input", this);
    treeState.addParameterListener("thresh", this);
    treeState.addParameterListener("ratio", this);
    treeState.addParameterListener("attack", this);
    treeState.addParameterListener("release", this);
    treeState.addParameterListener("output", this);
    treeState.addParameterListener("panner", this);
    treeState.addParameterListener("fader", this);



}

BasicCompAudioProcessor::~BasicCompAudioProcessor()
{
    treeState.removeParameterListener("input", this);
    treeState.removeParameterListener("thresh", this);
    treeState.removeParameterListener("ratio", this);
    treeState.removeParameterListener("attack", this);
    treeState.removeParameterListener("release", this);
    treeState.removeParameterListener("output", this);
    treeState.removeParameterListener("panner", this);
    treeState.removeParameterListener("fader", this);

}

juce::AudioProcessorValueTreeState::ParameterLayout BasicCompAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(5.0f, 2000.0f, 0.1f);
    releaseRange.setSkewForCentre(200.0f);
    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.01f, 100.0f, 0.1f);
    attackRange.setSkewForCentre(5.0f);

    auto pInput = std::make_unique<juce::AudioParameterFloat>("input", "Input", -60.0f, 10.0f, 0.0f);
    auto pThresh = std::make_unique<juce::AudioParameterFloat>("thresh", "Thresh", -60.0f, 10.0f, 0.0f);
    auto pRatio = std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio", 1.0f, 20.0f, 1.0f);
    auto pAttack = std::make_unique<juce::AudioParameterFloat>("attack", "Attack", attackRange, 10.0f);
    auto pRelease = std::make_unique<juce::AudioParameterFloat>("release", "Release", releaseRange, 125.0f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>("output", "Output", 0.0f, 60.0f, 0.0f);
    auto pPanner = std::make_unique<juce::AudioParameterFloat>("panner", "Panner", -1.0f, 1.0f, 0.0f);
    auto pFader = std::make_unique<juce::AudioParameterFloat>("fader", "Fader", -90.0f, 10.0f, 0.0f);


    params.push_back(std::move(pInput));
    params.push_back(std::move(pThresh));
    params.push_back(std::move(pRatio));
    params.push_back(std::move(pAttack));
    params.push_back(std::move(pRelease));
    params.push_back(std::move(pOutput));
    params.push_back(std::move(pPanner));
    params.push_back(std::move(pFader));



    return { params.begin(), params.end() };
}

void BasicCompAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    updateParameters();
}

void BasicCompAudioProcessor::updateParameters()
{
    inputModule.setGainDecibels(treeState.getRawParameterValue("input")->load());
    compressorModule.setThreshold(treeState.getRawParameterValue("thresh")->load());
    compressorModule.setRatio(treeState.getRawParameterValue("ratio")->load());
    compressorModule.setAttack(treeState.getRawParameterValue("attack")->load());
    compressorModule.setRelease(treeState.getRawParameterValue("release")->load());
    outputModule.setGainDecibels(treeState.getRawParameterValue("output")->load());
    pannerModule.setPan(treeState.getRawParameterValue("panner")->load());
    faderModule.setGainDecibels(treeState.getRawParameterValue("fader")->load());
}

//==============================================================================
const juce::String BasicCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicCompAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BasicCompAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool BasicCompAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double BasicCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicCompAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BasicCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicCompAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String BasicCompAudioProcessor::getProgramName(int index)
{
    return {};
}

void BasicCompAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void BasicCompAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    inputModule.prepare(spec);
    inputModule.setRampDurationSeconds(0.01f);
    outputModule.prepare(spec);
    outputModule.setRampDurationSeconds(0.01f);
    compressorModule.prepare(spec);
    pannerModule.prepare(spec);
    //*Set the Panner Rule
    //pannerModule.setRule(juce::dsp::PannerRule::squareRoot3dB);
    faderModule.prepare(spec);
    faderModule.setRampDurationSeconds(0.01f);

    updateParameters();

}

void BasicCompAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicCompAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void BasicCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> block{ buffer };
    inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    compressorModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    pannerModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    faderModule.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool BasicCompAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicCompAudioProcessor::createEditor()
{
    return new BasicCompAudioProcessorEditor(*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void BasicCompAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save Params
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void BasicCompAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    //Recall Params
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid())
    {
        treeState.state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicCompAudioProcessor();
}




//#include "PluginProcessor.h"
//#include "PluginEditor.h"
//
////==============================================================================
//BasicCompAudioProcessor::BasicCompAudioProcessor()
//#ifndef JucePlugin_PreferredChannelConfigurations
//     : AudioProcessor (BusesProperties()
//                     #if ! JucePlugin_IsMidiEffect
//                      #if ! JucePlugin_IsSynth
//                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
//                      #endif
//                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
//                     #endif
//                       )
//    , treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
//#endif
//{
//    treeState.addParameterListener("input", this);
//    treeState.addParameterListener("thresh", this);
//    treeState.addParameterListener("ratio", this);
//    treeState.addParameterListener("attack", this);
//    treeState.addParameterListener("release", this);
//    treeState.addParameterListener("output", this);
//}
//
//BasicCompAudioProcessor::~BasicCompAudioProcessor()
//{
//    treeState.removeParameterListener("input", this);
//    treeState.removeParameterListener("thresh", this);
//    treeState.removeParameterListener("ratio", this);
//    treeState.removeParameterListener("attack", this);
//    treeState.removeParameterListener("release", this);
//    treeState.removeParameterListener("output", this);
//}
//
//juce::AudioProcessorValueTreeState::ParameterLayout BasicCompAudioProcessor::createParameterLayout() {
//    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
//    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(5.0f, 2000.0f, 0.1f);
//    releaseRange.setSkewForCentre(200.0f);
//    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.01f, 100.0f, 0.1f);
//    attackRange.setSkewForCentre(5.0f);
//
//    auto pInput = std::make_unique<juce::AudioParameterFloat>("input", "Input", -60.0f, 10.0f, 0.0f);
//    auto pThresh = std::make_unique<juce::AudioParameterFloat>("thresh", "Thresh", -60.0f, 10.0f, 0.0f);
//    auto pRatio = std::make_unique<juce::AudioParameterFloat>("ratio", "Ratio", 1.0f, 20.0f, 1.0f);
//    auto pAttack = std::make_unique<juce::AudioParameterFloat>("attack", "Attack", attackRange, 10.0f);
//    auto pRelease = std::make_unique<juce::AudioParameterFloat>("release", "Release", releaseRange, 125.0f);
//    auto pOutput = std::make_unique<juce::AudioParameterFloat>("output", "Output", 0.0f, 60.0f, 0.0f);
//
//    params.push_back(std::move(pInput));
//    params.push_back(std::move(pThresh));
//    params.push_back(std::move(pRatio));
//    params.push_back(std::move(pAttack));
//    params.push_back(std::move(pRelease));
//    params.push_back(std::move(pOutput));
//
//    return { begin(params), end(params) };
//}
//
//void BasicCompAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue) {
//    updateParameters();
//}
//
//void BasicCompAudioProcessor::updateParameters() {
//    inputModule.setGainDecibels(treeState.getRawParameterValue("input")->load());
//    compressorModule.setThreshold(treeState.getRawParameterValue("thresh")->load());
//    compressorModule.setRatio(treeState.getRawParameterValue("ratio")->load());
//    compressorModule.setAttack(treeState.getRawParameterValue("attack")->load());
//    compressorModule.setRelease(treeState.getRawParameterValue("release")->load());
//    outputModule.setGainDecibels(treeState.getRawParameterValue("output")->load());
//}
//
//
////==============================================================================
//const juce::String BasicCompAudioProcessor::getName() const
//{
//    return JucePlugin_Name;
//}
//
//bool BasicCompAudioProcessor::acceptsMidi() const
//{
//   #if JucePlugin_WantsMidiInput
//    return true;
//   #else
//    return false;
//   #endif
//}
//
//bool BasicCompAudioProcessor::producesMidi() const
//{
//   #if JucePlugin_ProducesMidiOutput
//    return true;
//   #else
//    return false;
//   #endif
//}
//
//bool BasicCompAudioProcessor::isMidiEffect() const
//{
//   #if JucePlugin_IsMidiEffect
//    return true;
//   #else
//    return false;
//   #endif
//}
//
//double BasicCompAudioProcessor::getTailLengthSeconds() const
//{
//    return 0.0;
//}
//
//int BasicCompAudioProcessor::getNumPrograms()
//{
//    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
//                // so this should be at least 1, even if you're not really implementing programs.
//}
//
//int BasicCompAudioProcessor::getCurrentProgram()
//{
//    return 0;
//}
//
//void BasicCompAudioProcessor::setCurrentProgram (int index)
//{
//}
//
//const juce::String BasicCompAudioProcessor::getProgramName (int index)
//{
//    return {};
//}
//
//void BasicCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
//{
//}
//
////==============================================================================
//void BasicCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
//{
//    juce::dsp::ProcessSpec spec;
//    spec.maximumBlockSize = samplesPerBlock;
//    spec.sampleRate = sampleRate;
//    spec.numChannels = getTotalNumOutputChannels();
//
//    inputModule.prepare(spec);
//    inputModule.setRampDurationSeconds(0.01f);
//    outputModule.prepare(spec);
//    outputModule.setRampDurationSeconds(0.01f);
//    compressorModule.prepare(spec);
//    updateParameters();
//}
//
//void BasicCompAudioProcessor::releaseResources()
//{
//    // When playback stops, you can use this as an opportunity to free up any
//    // spare memory, etc.
//}
//
//#ifndef JucePlugin_PreferredChannelConfigurations
//bool BasicCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
//{
//  #if JucePlugin_IsMidiEffect
//    juce::ignoreUnused (layouts);
//    return true;
//  #else
//    // This is the place where you check if the layout is supported.
//    // In this template code we only support mono or stereo.
//    // Some plugin hosts, such as certain GarageBand versions, will only
//    // load plugins that support stereo bus layouts.
//    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
//     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
//        return false;
//
//    // This checks if the input layout matches the output layout
//   #if ! JucePlugin_IsSynth
//    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
//        return false;
//   #endif
//
//    return true;
//  #endif
//}
//#endif
//
//void BasicCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
//{
//    juce::ScopedNoDenormals noDenormals;
//    auto totalNumInputChannels  = getTotalNumInputChannels();
//    auto totalNumOutputChannels = getTotalNumOutputChannels();
//
//    juce::dsp::AudioBlock<float> block{ buffer };
//    inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
//    compressorModule.process(juce::dsp::ProcessContextReplacing<float>(block));
//    outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
//}
//
////==============================================================================
//bool BasicCompAudioProcessor::hasEditor() const
//{
//    return true; // (change this to false if you choose to not supply an editor)
//}
//
//juce::AudioProcessorEditor* BasicCompAudioProcessor::createEditor()
//{
//    //return new BasicCompAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor (*this);
//}
//
////==============================================================================
//void BasicCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
//{
//    juce::MemoryOutputStream stream(destData, false);
//    treeState.state.writeToStream(stream);
//}
//
//void BasicCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
//{
//    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
//    if (tree.isValid()) {
//        treeState.state = tree;
//    }
//}
//
////==============================================================================
//// This creates new instances of the plugin..
//juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
//{
//    return new BasicCompAudioProcessor();
//}
