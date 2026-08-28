#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SliderLabel : public juce::Label
{
public:
    using Label::Label;

    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = juce::Label::createEditorComponent();

        ed->setJustification (juce::Justification::centred);

        ed->setIndents (0, 0);

        ed->setColour (juce::TextEditor::highlightColourId, juce::Colours::transparentBlack);

        return ed;
    }
};


class DispenserLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    DispenserLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    juce::Label* createSliderTextBox (juce::Slider&) override;
    void fillTextEditorBackground (juce::Graphics&, int width, int height, juce::TextEditor&) override;
    void drawTextEditorOutline (juce::Graphics&, int width, int height, juce::TextEditor&) override;

    void drawLabel (juce::Graphics& g, juce::Label& label) override
    {
        g.fillAll (label.findColour (juce::Label::backgroundColourId));

        if (! label.isBeingEdited())
        {
            const auto alpha = label.isEnabled() ? 1.0f : 0.5f;
            g.setColour (label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha));
            g.setFont (label.getFont());

            const auto textArea = label.getBorderSize().subtractedFrom (label.getLocalBounds());

            g.drawText (label.getText(), textArea,
                        label.getJustificationType(),
                        false);
        }
    }

    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
};
