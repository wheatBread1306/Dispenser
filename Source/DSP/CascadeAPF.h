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
    alignas(32) std::array<float, 8> s1L{};
    alignas(32) std::array<float, 8> s2L{};
    alignas(32) std::array<float, 8> s1R{};
    alignas(32) std::array<float, 8> s2R{};

    alignas(32) std::array<float, 8> stageOutHistoryL{};
    alignas(32) std::array<float, 8> stageOutHistoryR{};

    juce::SmoothedValue<float> freqSmoothed{};
    juce::SmoothedValue<float> resSmoothed{};
    juce::SmoothedValue<float> driftSmoothed{};

    double currentSampleRate = 44100.0;
    float maxFrequency = {};
    float freqToTanScaler = {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CascadeAPF)
};
