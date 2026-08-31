#pragma once

#include <juce_dsp/juce_dsp.h>

class CascadeAPF final
{
public:
    CascadeAPF() = default;

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;
    void process(const juce::dsp::AudioBlock<float>& block) noexcept;
    void setParams(float frequency, float resonance, float drift) noexcept;
    void reset() noexcept;

    [[nodiscard]] static size_t getLatency() noexcept { return 7; }

private:

    struct filterHistory final
    {
        alignas(32) std::array<float, 8> s1{};
        alignas(32) std::array<float, 8> s2{};
        alignas(32) std::array<float, 8> stageOutHistory{};

        void reset() noexcept
        {
            for (size_t i = 0; i < 8; ++i)
            {
                s1[i] = 0.0f;
                s2[i] = 0.0f;
                stageOutHistory[i] = 0.0f;
            }
        }
    };

    void processFilterImpl(
        float* __restrict buffer,
        filterHistory& hist,
        const std::array<float, 8>& driftTable,
        size_t numSamples) const noexcept;

    filterHistory lCh{};
    filterHistory rCh{};

    juce::SmoothedValue<float> freqSmoothed{};
    juce::SmoothedValue<float> resSmoothed{};
    juce::SmoothedValue<float> driftSmoothed{};

    double currentSampleRate = 44100.0;
    float maxFrequency = {};
    float freqToTanScaler = {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CascadeAPF)
};
