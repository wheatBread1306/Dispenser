/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/Manager.h"
#include "Parameters/PluginParameters.h"

//==============================================================================
/**
*/
class DispenserAudioProcessor : public juce::AudioProcessor
{
public:
    DispenserAudioProcessor();
    ~DispenserAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override
    {
    }

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void reset() override;

    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "Parameters", Parameters::createParameterLayout()};

protected:
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

private:
    juce::AudioProcessorEditor* createEditor() override;

    Manager apf{};

    std::atomic<float>* levelParam{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DispenserAudioProcessor)
};
