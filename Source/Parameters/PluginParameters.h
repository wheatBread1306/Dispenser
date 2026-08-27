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


    // パラメータのセットアップ関数
    inline juce::AudioProcessorValueTreeState::ParameterLayout
    createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterInt>(LEVEL_ID, LEVEL_NAME, 1, 8, 1));


        return layout;
    }
}
