#pragma once

#include <juce_dsp/juce_dsp.h>

#if defined(_MSC_VER)
// MSVC or clang-cl in MSVC mode
#define FORCE_INLINE __forceinline
#elif defined(__clang__)
// Clang native
#define FORCE_INLINE [[clang::always_inline]] inline
#endif

template<size_t CHUNK_SIZE = 256>
class Gain final
{
public:
    Gain() = default;
    ~Gain() = default;

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate;
        gain.reset(spec.sampleRate, 0.01);
    }

    FORCE_INLINE void process(const juce::dsp::AudioBlock<float>& block) noexcept
    {
        const size_t fullContextSize = block.getNumSamples();
        for (size_t chunkStart = 0; chunkStart < fullContextSize; chunkStart += CHUNK_SIZE)
        {
            const size_t chunkSize = std::min(CHUNK_SIZE, fullContextSize - chunkStart);
            std::array<float, CHUNK_SIZE> gainCoef{};
            for (size_t i = 0; i < chunkSize; ++i)
            {
                gainCoef[i] = gain.getNextValue();
            }

            for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
            {
                auto* __restrict in = block.getChannelPointer(ch) + chunkStart;
                for (size_t i = 0; i < chunkSize; ++i)
                {
                    const float inI = in[i];
                    in[i] = inI * gainCoef[i];
                }
            }
        }
    }

    void reset() noexcept
    {
        gain.reset(sampleRate, 0.01);
    }

    FORCE_INLINE void setGain(float const newGain) noexcept { gain.setTargetValue(newGain); }

private:
    juce::SmoothedValue<float> gain;
    double sampleRate = 44100;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Gain)
};