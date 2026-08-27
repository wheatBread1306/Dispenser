/*
  ==============================================================================

    PluginParameters.h
    パラメータID定義ファイル

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace Parameters
{
    constexpr auto LEVEL_ID = "lev";
    constexpr auto LEVEL_NAME = "LEVEL";

    constexpr auto FREQUENCY_ID = "fq";
    constexpr auto FREQUENCY_NAME = "FREQUENCY";

    constexpr auto RESONANCE_ID = "res";
    constexpr auto RESONANCE_NAME = "RESONANCE";

    constexpr auto DRIFT_ID = "dr";
    constexpr auto DRIFT_NAME = "DRIFT";

    constexpr auto CLIP_ID = "cl";
    constexpr auto CLIP_NAME = "CLIP AT 0DB";

    inline juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterInt>(LEVEL_ID, LEVEL_NAME, 1, 8, 1));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            FREQUENCY_ID, FREQUENCY_NAME,
            juce::NormalisableRange<float>(40.0f, 20000.0f, 0.1f, 0.2f), 1000.0f,
            juce::String(), juce::AudioProcessorParameter::genericParameter,
            [](const float value, int)
            {
                return juce::String(value, 2);
            },
            [](const juce::String& text)
            {
                return text.getFloatValue();
            }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            RESONANCE_ID, RESONANCE_NAME,
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.7f), 0.707f,
            juce::String(), juce::AudioProcessorParameter::genericParameter,
            [](const float value, int)
            {
                return juce::String(value, 2);
            },
            [](const juce::String& text)
            {
                return text.getFloatValue();
            }));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            DRIFT_ID, DRIFT_NAME,
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f,
            juce::String(), juce::AudioProcessorParameter::genericParameter,
            [](const float value, int)
            {
                return juce::String(value, 2);
            },
            [](const juce::String& text)
            {
                return text.getFloatValue();
            }));

        layout.add(std::make_unique<juce::AudioParameterBool>(CLIP_ID, CLIP_NAME, false));

        return layout;
    }
}
