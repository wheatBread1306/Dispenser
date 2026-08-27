#include "APFCore.h"

void APFCore::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    sampleRate = spec.sampleRate;
    invSampleRate = 1.0f / sampleRate;

    maxFrequency = static_cast<float>(sampleRate) * 0.49f;
    freqToTanScaler = juce::MathConstants<float>::pi * static_cast<float>(invSampleRate);

    reset();
}

void APFCore::setParams(const float frequency, const float resonance) noexcept
{
    const float limitedFrequency = std::clamp(frequency, 0.0f, maxFrequency);

    const float x = limitedFrequency * freqToTanScaler;
    const float g = x * (1.0f + (x * x) * 0.333333f); // tan近似

    // FIX: Use std::max to enforce a lower bound for resonance
    const float k = 1.0f / std::max(resonance, 0.001f);

    const float denominator = 1.0f + g * (g + k);
    const float invDenominator = 1.0f / denominator;

    a1 = invDenominator;
    a2 = g * invDenominator;
    a3 = g * a2;
    currentK = k;
}

void APFCore::process(const juce::dsp::AudioBlock<float>& block) noexcept
{
    const auto ch = std::min(block.getNumChannels(), 2uz);
    const auto len = block.getNumSamples();

    const float ca1 = a1;
    const float ca2 = a2;
    const float ca3 = a3;
    const float cK = currentK;

    for (size_t i = 0; i < ch; ++i)
    {
        float s1v = s1[i];
        float s2v = s2[i];

        auto* __restrict buffer = block.getChannelPointer(i);

        for (size_t s = 0; s < len; ++s)
        {
            // TPT SVF Core Algorithm
            const float v3 = buffer[s] - s2v;
            const float v1 = ca1 * s1v + ca2 * v3;
            const float v2 = s2v + ca2 * s1v + ca3 * v3;

            s1v = std::fma(2.0f, v1, -s1v);
            s2v = std::fma(2.0f, v2, -s2v);

            const float bp = v1;

            const float ap = buffer[s] - 2.0f * cK * bp;

            buffer[s] = ap;
        }

        s1[i] = s1v;
        s2[i] = s2v;
    }
}

float APFCore::processSample(const float buffer, const size_t ch) noexcept
{
    const float ca1 = a1;
    const float ca2 = a2;
    const float ca3 = a3;
    const float cK = currentK;

    const float s1v = s1[ch];
    const float s2v = s2[ch];

    // TPT SVF Core Algorithm
    const float v3 = buffer - s2v;
    const float v1 = ca1 * s1v + ca2 * v3;
    const float v2 = s2v + ca2 * s1v + ca3 * v3;

    s1[ch] = std::fma(2.0f, v1, -s1v);
    s2[ch] = std::fma(2.0f, v2, -s2v);

    const float bp = v1;

    return buffer - 2.0f * cK * bp;
}

void APFCore::reset() noexcept
{
    s1.fill(0.0f);
    s2.fill(0.0f);
}
