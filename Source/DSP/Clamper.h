#pragma once

#include <juce_dsp/juce_dsp.h>

class Clamper final
{
public:
    Clamper() = delete;

    static void process(const juce::dsp::AudioBlock<float>& block) noexcept
    {
        const auto numChannels = block.getNumChannels();
        const auto numSamples = block.getNumSamples();

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            const auto buffer = block.getChannelPointer(ch);

            for (size_t s = 0; s < numSamples; ++s)
            {
                buffer[s] = std::clamp(buffer[s], -1.0f, 1.0f);
            }
        }
    }
};
