#pragma once

#include <array>
#include <memory>
#include <utility>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

class DispenserPanel final : public juce::Component
{
public:
    explicit DispenserPanel(DispenserAudioProcessor&);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    DispenserAudioProcessor& processor;
    juce::Slider levelSlider;
    juce::Slider frequencySlider;
    juce::Slider resonanceSlider;
    juce::Slider driftSlider;
    juce::Slider preGainSlider;
    juce::Slider postGainSlider;
    juce::ToggleButton clipButton { "CLIP AT 0 dB" };

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    std::array<std::unique_ptr<juce::Label>, 6> valueLabels;
    std::unique_ptr<SliderAttachment> levelAttachment;
    std::unique_ptr<SliderAttachment> frequencyAttachment;
    std::unique_ptr<SliderAttachment> resonanceAttachment;
    std::unique_ptr<SliderAttachment> driftAttachment;
    std::unique_ptr<SliderAttachment> preGainAttachment;
    std::unique_ptr<SliderAttachment> postGainAttachment;
    std::unique_ptr<ButtonAttachment> clipAttachment;

    void configureSlider(juce::Slider&, const juce::String&);
    static void setSliderBounds(juce::Slider&, juce::Rectangle<int>);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DispenserPanel)
};
