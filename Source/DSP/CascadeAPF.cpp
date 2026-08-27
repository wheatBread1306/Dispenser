#include "CascadeAPF.h"

void CascadeAPF::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    sampleRate = spec.sampleRate;
    invSampleRate = 1.0f / sampleRate;

    maxFrequency = static_cast<float>(sampleRate) * 0.49f;
    freqToTanScaler = juce::MathConstants<float>::pi * static_cast<float>(invSampleRate);

    reset();
}

void CascadeAPF::setParams(const float frequency, const float resonance) noexcept
{
    const float limitedFrequency = std::clamp(frequency, 0.0f, maxFrequency);

    const float x = limitedFrequency * freqToTanScaler;
    const float g = x * (1.0f + (x * x) * 0.333333f); // tan近似

    // FIX: Use std::max to enforce a lower bound for resonance
    const float k = 1.0f / std::max(resonance, 0.001f);

    const float denominator = 1.0f + g * (g + k);
    const float invDenominator = 1.0f / denominator;

    a1.fill(invDenominator);
    a2.fill(g * invDenominator);
    a3.fill(g * g * invDenominator);
    currentK.fill(k);
}

void CascadeAPF::process(const juce::dsp::AudioBlock<float>& block) noexcept
{
    const auto numChannels = std::min(block.getNumChannels(), 2uz);
    const auto numSamples = block.getNumSamples();

    {
        auto* __restrict buffer = block.getChannelPointer(0);

        for (size_t s = 0; s < numSamples; ++s)
        {
            std::array<float, 8> stageInputs{};
            stageInputs[0] = buffer[s];
            for (size_t i = 1; i < 8; ++i)
            {
                stageInputs[i] = stageOutHistoryL[i - 1];
            }

            for (size_t i = 0; i < 8; ++i)
            {
                const float v3 = stageInputs[i] - s2L[i];
                const float v1 = a1[i] * s1L[i] + a2[i] * v3;
                const float v2 = s2L[i] + a2[i] * s1L[i] + a3[i] * v3;

                s1L[i] = 2.0f * v1 - s1L[i];
                s2L[i] = 2.0f * v2 - s2L[i];

                stageOutHistoryL[i] = stageInputs[i] - 2.0f * currentK[i] * v1;
            }

            buffer[s] = stageOutHistoryL[7];
        }
    }

    if (numChannels < 2)
        return;

    {
        auto* __restrict buffer = block.getChannelPointer(1);

        for (size_t s = 0; s < numSamples; ++s)
        {
            std::array<float, 8> stageInputs{};
            stageInputs[0] = buffer[s];
            for (size_t i = 1; i < 8; ++i)
            {
                stageInputs[i] = stageOutHistoryR[i - 1];
            }

            for (size_t i = 0; i < 8; ++i)
            {
                const float v3 = stageInputs[i] - s2R[i];
                const float v1 = a1[i] * s1R[i] + a2[i] * v3;
                const float v2 = s2R[i] + a2[i] * s1R[i] + a3[i] * v3;

                s1R[i] = 2.0f * v1 - s1R[i];
                s2R[i] = 2.0f * v2 - s2R[i];

                stageOutHistoryR[i] = stageInputs[i] - 2.0f * currentK[i] * v1;
            }
            buffer[s] = stageOutHistoryR[7];
        }
    }
}

void CascadeAPF::reset() noexcept
{
    s1L.fill(0.0f);
    s2L.fill(0.0f);
    s1R.fill(0.0f);
    s2R.fill(0.0f);
}
