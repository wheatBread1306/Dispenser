#include "CascadeAPF.h"

void CascadeAPF::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    currentSampleRate = spec.sampleRate;
    maxFrequency = static_cast<float>(spec.sampleRate) * 0.49f;
    freqToTanScaler = juce::MathConstants<float>::pi * static_cast<float>(1.0 / spec.sampleRate);

    reset();
}

void CascadeAPF::setParams(const float frequency, const float resonance, const float drift) noexcept
{
    freqSmoothed.setTargetValue(frequency);
    resSmoothed.setTargetValue(resonance);
    driftSmoothed.setTargetValue(drift);
}

void CascadeAPF::process(const juce::dsp::AudioBlock<float>& block) noexcept
{
    const auto numChannels = std::min(block.getNumChannels(), 2uz);
    const auto numSamples = block.getNumSamples();

    auto dL = driftSmoothed;
    auto fL = freqSmoothed;
    auto rL = resSmoothed;

    {
        static constexpr float driftTable[8] = {
            -0.12f, 0.05f, -0.08f, 0.15f,
            -0.03f, 0.11f, -0.07f, 0.02f
        };

        auto* __restrict buffer = block.getChannelPointer(0);

        for (size_t s = 0; s < numSamples; ++s)
        {
            const float driftIn = dL.getNextValue();
            const float freqIn = fL.getNextValue();
            const float resIn = rL.getNextValue();
            const float k = 1.0f / std::max(resIn, 0.001f);

            std::array<float, 8> a1{};
            std::array<float, 8> a2{};
            std::array<float, 8> a3{};
            std::array<float, 8> currentK{};

            for (size_t i = 0; i < 8; ++i)
            {
                const float detune = fast_exp2(driftIn * driftTable[i] * 6.37f);
                const float driftedFrequency = freqIn * detune;

                const float limitedFrequency = std::clamp(driftedFrequency, 20.0f, maxFrequency);

                const float x = limitedFrequency * freqToTanScaler;
                const float g = x * (1.0f + (x * x) * 0.333333f);

                const float denominator = 1.0f + g * (g + k);
                const float invDenominator = 1.0f / denominator;

                a1[i] = invDenominator;
                a2[i] = (g * invDenominator);
                a3[i] = (g * g * invDenominator);
                currentK[i] = (k);
            }

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
    {
        driftSmoothed = dL;
        freqSmoothed = fL;
        resSmoothed = rL;
        return;
    }

    auto dR = driftSmoothed;
    auto fR = freqSmoothed;
    auto rR = resSmoothed;

    {
        auto* __restrict buffer = block.getChannelPointer(1);

        static constexpr float driftTable[8] = {
            -0.12f, -0.08f, 0.11f, -0.03f,
            0.15f, -0.07f, 0.02f, -0.07f
        };

        for (size_t s = 0; s < numSamples; ++s)
        {
            const float driftIn = dR.getNextValue();
            const float freqIn = fR.getNextValue();
            const float resIn = rR.getNextValue();
            const float k = 1.0f / std::max(resIn, 0.001f);

            std::array<float, 8> a1{};
            std::array<float, 8> a2{};
            std::array<float, 8> a3{};
            std::array<float, 8> currentK{};

            for (size_t i = 0; i < 8; ++i)
            {
                const float detune = fast_exp2(driftIn * driftTable[i] * 6.37f);
                const float driftedFrequency = freqIn * detune;

                const float limitedFrequency = std::clamp(driftedFrequency, 20.0f, maxFrequency);

                const float x = limitedFrequency * freqToTanScaler;
                const float g = x * (1.0f + (x * x) * 0.333333f);

                const float denominator = 1.0f + g * (g + k);
                const float invDenominator = 1.0f / denominator;

                a1[i] = invDenominator;
                a2[i] = (g * invDenominator);
                a3[i] = (g * g * invDenominator);
                currentK[i] = (k);
            }

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

    driftSmoothed = dR;
    freqSmoothed = fR;
    resSmoothed = rR;
}

void CascadeAPF::reset() noexcept
{
    s1L.fill(0.0f);
    s2L.fill(0.0f);
    s1R.fill(0.0f);
    s2R.fill(0.0f);

    freqSmoothed.reset(currentSampleRate, 0.01);
    resSmoothed.reset(currentSampleRate, 0.01);
    driftSmoothed.reset(currentSampleRate, 0.01);

    stageOutHistoryL.fill(0.0f);
    stageOutHistoryR.fill(0.0f);
}
