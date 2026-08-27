/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DispenserAudioProcessor::DispenserAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    levelParam = apvts.getRawParameterValue(Parameters::LEVEL_ID);
    freqParam = apvts.getRawParameterValue(Parameters::FREQUENCY_ID);
    qParam = apvts.getRawParameterValue(Parameters::RESONANCE_ID);
}

DispenserAudioProcessor::~DispenserAudioProcessor()
= default;

//==============================================================================
const juce::String DispenserAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DispenserAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DispenserAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DispenserAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DispenserAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DispenserAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int DispenserAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DispenserAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String DispenserAudioProcessor::getProgramName(int index)
{
    return {};
}

void DispenserAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void DispenserAudioProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    const juce::dsp::ProcessSpec spec{
        .sampleRate = sampleRate, .maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock),
        .numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels())
    };
    apf.prepare(spec);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DispenserAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void DispenserAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const juce::dsp::AudioBlock<float> block(buffer);

    const auto l = static_cast<size_t>(levelParam->load(std::memory_order_relaxed));
    if (l != apf.getLevel())
    {
        apf.setLevel(l);
        setLatencySamples(apf.getLatency());
    }

    apf.setParams(freqParam->load(std::memory_order_relaxed), qParam->load(std::memory_order_relaxed));
    apf.process(block);
}

void DispenserAudioProcessor::reset()
{
    apf.reset();
}

//==============================================================================
bool DispenserAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DispenserAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void DispenserAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    const std::unique_ptr xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void DispenserAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DispenserAudioProcessor();
}
