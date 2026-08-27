#pragma once

#include <juce_dsp/juce_dsp.h>

class CascadeAPF final
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;
    void process(const juce::dsp::AudioBlock<float>& block) noexcept;
    void setParams(float frequency, float resonance) noexcept;
    void reset() noexcept;

    [[nodiscard]] static size_t getLatency() noexcept {return 8;}
private:
    std::array<float, 8> s1L{};
    std::array<float, 8> s2L{};

    double sampleRate = 44100.0;
    double invSampleRate = 1 / 44100.0;
    
    std::array<float, 8> maxFrequency = {};
    std::array<float, 8> freqToTanScaler = {};
    std::array<float, 8> a1 = {};
    std::array<float, 8> a2 = {};
    std::array<float, 8> a3 = {};
    std::array<float, 8> currentK = {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CascadeAPF)
};
