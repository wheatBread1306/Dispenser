#pragma once

#include <juce_dsp/juce_dsp.h>
#include "CascadeAPF.h"

class Manager final
{
public:
    Manager() = default;

    void prepare(const juce::dsp::ProcessSpec& spec) noexcept;
    void process(const juce::dsp::AudioBlock<float>& block) noexcept;
    void setParams(float frequency, float resonance, float drift) noexcept;
    void reset() noexcept;

    void setLevel(const size_t newLevel) noexcept
    {
        level = std::clamp(newLevel, 1uz, 8uz);
    }

    auto getLevel() const noexcept
    {
        return level;
    }

    [[nodiscard]] size_t getLatency() const noexcept { return level * 7; }

private:
    std::array<CascadeAPF, 8> apfs{};
    size_t level{1};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Manager)
};
