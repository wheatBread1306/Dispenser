#pragma once

#include <juce_dsp/juce_dsp.h>

class CascadeAPF final
{
public:
    CascadeAPF() = default;

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;
    void process(const juce::dsp::AudioBlock<float>& block) noexcept;
    void setParams(float frequency, float resonance) noexcept;
    void reset() noexcept;

    [[nodiscard]] static size_t getLatency() noexcept {return 8;}
private:
    std::array<float, 8> a1 = {};
    std::array<float, 8> a2 = {};
    std::array<float, 8> a3 = {};
    std::array<float, 8> currentK = {};

    std::array<float, 8> s1L{};
    std::array<float, 8> s2L{};
    std::array<float, 8> s1R{};
    std::array<float, 8> s2R{};

    std::array<float, 8> stageOutHistoryL{};
    std::array<float, 8> stageOutHistoryR{};

    double sampleRate = 44100.0;
    double invSampleRate = 1 / 44100.0;

    float maxFrequency = {};
    float freqToTanScaler = {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CascadeAPF)
};
