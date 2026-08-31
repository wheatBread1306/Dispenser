#include "CascadeAPF.h"

#if defined(_MSC_VER)
// MSVC or clang-cl in MSVC mode
#define FORCE_INLINE __forceinline
#elif defined(__clang__)
// Clang native
#define FORCE_INLINE [[clang::always_inline]] inline
#endif

namespace
{
    FORCE_INLINE float fast_exp2(float const x) noexcept
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

    constexpr std::array driftTableL = {
        -0.12f, -0.08f, 0.11f, -0.03f,
        0.15f, -0.07f, 0.02f, -0.07f
    };

    constexpr std::array driftTableR = {
        -0.12f, 0.05f, -0.08f, 0.15f,
        -0.03f, 0.11f, -0.07f, 0.02f
    };
}

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

FORCE_INLINE void CascadeAPF::processFilterImpl(
    float* __restrict buffer,
    filterHistory& hist,
    const std::array<float, 8>& driftTable,
    const size_t numSamples) const noexcept
{
    auto dIn = driftSmoothed;
    auto fIn = freqSmoothed;
    auto rIn = resSmoothed;

    for (size_t s = 0; s < numSamples; ++s)
    {
        const float driftIn = dIn.getNextValue();
        const float freqIn = fIn.getNextValue();
        const float resIn = rIn.getNextValue();
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
            stageInputs[i] = hist.stageOutHistory[i - 1];
        }

        for (size_t i = 0; i < 8; ++i)
        {
            const float v3 = stageInputs[i] - hist.s2[i];
            const float v1 = a1[i] * hist.s1[i] + a2[i] * v3;
            const float v2 = hist.s2[i] + a2[i] * hist.s1[i] + a3[i] * v3;

            hist.s1[i] = 2.0f * v1 - hist.s1[i];
            hist.s2[i] = 2.0f * v2 - hist.s2[i];

            hist.stageOutHistory[i] = stageInputs[i] - 2.0f * currentK[i] * v1;
        }
        buffer[s] = hist.stageOutHistory[7];
    }
}

void CascadeAPF::process(const juce::dsp::AudioBlock<float>& block) noexcept
{
    const auto numChannels = std::min(block.getNumChannels(), 2uz);
    const auto numSamples = block.getNumSamples();

    auto* __restrict bufferL = block.getChannelPointer(0);
    processFilterImpl(bufferL, lCh, driftTableL, numSamples);

    if (numChannels >= 2) [[likely]]
    {
        auto* __restrict bufferR = block.getChannelPointer(1);
        processFilterImpl(bufferR, rCh, driftTableR, numSamples);
    }

    driftSmoothed.skip(static_cast<int>(numSamples));
    freqSmoothed.skip(static_cast<int>(numSamples));
    resSmoothed.skip(static_cast<int>(numSamples));
}

void CascadeAPF::reset() noexcept
{
    lCh.reset();
    rCh.reset();

    freqSmoothed.reset(currentSampleRate, 0.01);
    resSmoothed.reset(currentSampleRate, 0.01);
    driftSmoothed.reset(currentSampleRate, 0.01);
}
