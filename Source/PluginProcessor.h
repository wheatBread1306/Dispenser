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
#include "DSP/Clamper.h"
#include "DSP/Gain.h"

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

    Gain<> pre{};
    Gain<> post{};

    std::atomic<float>* levelParam{nullptr};
    std::atomic<float>* freqParam{nullptr};
    std::atomic<float>* qParam{nullptr};
    std::atomic<float>* driftParam{nullptr};
    std::atomic<float>* clipParam{nullptr};
    std::atomic<float>* preParam{nullptr};
    std::atomic<float>* postParam{nullptr};

    FORCE_INLINE static float fast_exp2(float const x) noexcept
    {
        const float i = std::floor(x);
        const float f = x - i;

        const float r = 1.0000000f + f * (0.6931472f + f * (0.2402265f + f * (0.0555041f + f * (0.0096181f + f *
            0.0013333f))));

        union
        {
            float f;
            int32_t i;
        } u{};
        u.f = r;
        u.i += static_cast<int32_t>(i) << 23;
        return u.f;
    }

    FORCE_INLINE static float fastDecibelsToGain(const float decibels) noexcept
    {
        if (decibels <= -100.0f) return 0.0f;

        return fast_exp2(decibels * 0.166096404744368f);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DispenserAudioProcessor)
};
