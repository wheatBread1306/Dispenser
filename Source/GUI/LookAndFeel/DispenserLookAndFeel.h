#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class DispenserLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    DispenserLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
};
