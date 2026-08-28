#include "DispenserPanel.h"
#include "../LookAndFeel/DispenserLookAndFeel.h"
#include "../../Parameters/PluginParameters.h"

DispenserPanel::DispenserPanel(DispenserAudioProcessor& processorToUse)
    : processor(processorToUse)
{
    titleLabel.setText("DISPENSER", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(27.0f).withStyle("bold"));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe8c477));
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("ADAPTIVE PHASE FILTER", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(11.0f));
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff82918e));
    addAndMakeVisible(subtitleLabel);

    configureSlider(levelSlider, "LEVEL");
    configureSlider(frequencySlider, "FREQUENCY");
    configureSlider(resonanceSlider, "RESONANCE");
    configureSlider(driftSlider, "DRIFT");
    configureSlider(preGainSlider, "PRE GAIN");
    configureSlider(postGainSlider, "POST GAIN");

    levelAttachment = std::make_unique<SliderAttachment>(processor.apvts, Parameters::LEVEL_ID, levelSlider);
    frequencyAttachment = std::make_unique<SliderAttachment>(processor.apvts, Parameters::FREQUENCY_ID, frequencySlider);
    resonanceAttachment = std::make_unique<SliderAttachment>(processor.apvts, Parameters::RESONANCE_ID, resonanceSlider);
    driftAttachment = std::make_unique<SliderAttachment>(processor.apvts, Parameters::DRIFT_ID, driftSlider);
    preGainAttachment = std::make_unique<SliderAttachment>(processor.apvts, Parameters::PRE_GAIN_ID, preGainSlider);
    postGainAttachment = std::make_unique<SliderAttachment>(processor.apvts, Parameters::POST_GAIN_ID, postGainSlider);

    clipButton.setClickingTogglesState(true);
    clipButton.setColour(juce::ToggleButton::textColourId, juce::Colour(0xffd9e2df));
    addAndMakeVisible(clipButton);
    clipAttachment = std::make_unique<ButtonAttachment>(processor.apvts, Parameters::CLIP_ID, clipButton);
}

void DispenserPanel::configureSlider(juce::Slider& slider, const juce::String& labelText)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 86, 22);
    slider.setTextValueSuffix(labelText == "FREQUENCY" ? " Hz" : labelText == "RESONANCE" ? " Q" : "");
    slider.setName(labelText);
    addAndMakeVisible(slider);
}

void DispenserPanel::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    g.fillAll(juce::Colour(0xff101416));
    g.setColour(juce::Colour(0xff1b2425));
    g.fillRoundedRectangle(bounds.reduced(12.0f), 8.0f);

    g.setColour(juce::Colour(0xffd5a85b));
    const float lineY = subtitleLabel.getBottom() + 2.0f;
    g.fillRect(28.0f, lineY, bounds.getWidth() - 56.0f, 2.0f);

    g.setColour(juce::Colour(0xff82918e));
    g.setFont(juce::FontOptions(10.0f));
    g.drawText("FILTER PARAMETERS", 28, lineY + 5.0f, bounds.getWidth() - 56, 16, juce::Justification::centredLeft);

    const std::array<std::pair<const juce::Slider*, juce::String>, 6> labels {{
        { &levelSlider, "LEVEL" }, { &frequencySlider, "FREQUENCY" },
        { &resonanceSlider, "RESONANCE" }, { &driftSlider, "DRIFT" },
        { &preGainSlider, "PRE GAIN" }, { &postGainSlider, "POST GAIN" }
    }};

    g.setColour(juce::Colour(0xffd9e2df));
    g.setFont(juce::FontOptions(11.0f));
    for (const auto& [slider, label] : labels)
    {
        auto r = slider->getBounds().toFloat();
        g.drawText(label, r.withHeight(20.0f).translated(0, -15.0f), juce::Justification::centred, false);
    }
}

void DispenserPanel::resized()
{
    const auto bounds = getLocalBounds().toFloat();
    const float w = bounds.getWidth();
    const float h = bounds.getHeight();
    auto area = bounds.reduced(w * 0.05f);

    auto headerArea = area.removeFromTop(h * 0.10f);
    titleLabel.setBounds(headerArea.removeFromTop(headerArea.getHeight() * 0.6f).toNearestInt());
    subtitleLabel.setBounds(headerArea.toNearestInt());

    area.removeFromTop(30.0f);

    auto mainRow = area.removeFromTop(h * 0.4f);
    const auto cellW4 = mainRow.getWidth() / 4.0f;

    auto setSlider = [](juce::Slider& s, const juce::Rectangle<float> r) {
        s.setBounds(r.reduced(5, 10).toNearestInt());
    };

    setSlider(levelSlider,     mainRow.removeFromLeft(cellW4));
    setSlider(frequencySlider, mainRow.removeFromLeft(cellW4));
    setSlider(resonanceSlider, mainRow.removeFromLeft(cellW4));
    setSlider(driftSlider,     mainRow);

    area.removeFromTop(h * 0.05f);

    auto bottomRow = area;
    const auto cellW3 = bottomRow.getWidth() / 3.0f;

    setSlider(preGainSlider,  bottomRow.removeFromLeft(cellW3));
    setSlider(postGainSlider, bottomRow.removeFromLeft(cellW3));

    const auto btnArea = bottomRow.reduced(10, bottomRow.getHeight() * 0.2f);
    clipButton.setBounds(btnArea.toNearestInt());
}

void DispenserPanel::setSliderBounds(juce::Slider& slider, const juce::Rectangle<int> bounds)
{
    slider.setBounds(bounds.reduced(8, 0));
}
