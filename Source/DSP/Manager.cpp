#include "Manager.h"

void Manager::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    for (auto& apf : apfs)
    {
        apf.prepare(spec);
    }
}

void Manager::process(const juce::dsp::AudioBlock<float>& block) noexcept
{
    for (size_t i = 0; i < level; ++i)
    {
        apfs[i].process(block);
    }
}

void Manager::setParams(const float frequency, const float resonance, const float drift) noexcept
{
    for (auto& apf : apfs)
    {
        apf.setParams(frequency,resonance,drift);
    }
}

void Manager::reset() noexcept
{
    for (auto& apf : apfs)
    {
        apf.reset();
    }
}
