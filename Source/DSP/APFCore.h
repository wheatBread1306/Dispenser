#pragma once

#include <juce_dsp/juce_dsp.h>

class APFCore final
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;
    void process(const juce::dsp::AudioBlock<float>& block) noexcept;
    float processSample(float buffer, size_t ch) noexcept;
    void setParams(float frequency, float resonance) noexcept;
    void reset() noexcept;

private:
    static constexpr size_t kMaxChannels = 2;

    double sampleRate = 44100.0;
    double invSampleRate = 1 / 44100.0;

    std::array<float, kMaxChannels> s1{};
    std::array<float, kMaxChannels> s2{};

    float maxFrequency = 0.0f;
    float freqToTanScaler = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;
    float a3 = 0.0f;
    float currentK = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(APFCore)
};
