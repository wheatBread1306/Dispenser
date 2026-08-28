#include "DispenserLookAndFeel.h"

DispenserLookAndFeel::DispenserLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff101416));
    setColour(juce::Label::textColourId, juce::Colour(0xffd9e2df));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffd9e2df));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
}

void DispenserLookAndFeel::drawRotarySlider(juce::Graphics& g, const int x, const int y, const int width,
                                            const int height,
                                            const float sliderPosProportional, const float rotaryStartAngle,
                                            const float rotaryEndAngle, juce::Slider& slider)
{
    const auto size = juce::jmin(width, height) - 10.0f;
    auto centerX = x + width * 0.5f;
    auto centerY = y + height * 0.5f;
    const auto ringBounds = juce::Rectangle(size, size).withCentre({centerX, centerY});

    const auto accentColor = juce::Colour(0xff4000c0);
    const auto trackColor = juce::Colours::whitesmoke.withAlpha(0.3f);
    const auto knobFaceColor = juce::Colours::black.withAlpha(0.2f);

    constexpr float lineThickness = 4.0f;
    const float radius = (size - lineThickness) * 0.5f;
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(trackColor);
    g.strokePath(track, juce::PathStrokeType(lineThickness));

    juce::Path valueArc;
    valueArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(accentColor);
    g.strokePath(valueArc, juce::PathStrokeType(lineThickness));

    g.setColour(knobFaceColor);
    g.fillEllipse(ringBounds.reduced(lineThickness + 1.0f));

    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawEllipse(ringBounds.reduced(lineThickness + 1.0f), 1.0f);

    g.setColour(juce::Colours::white);
    const float dotSize = 4.0f;
    const float dotRadius = radius;
    const float dotX = centerX + std::sin(angle) * dotRadius;
    const float dotY = centerY - std::cos(angle) * dotRadius;
    g.fillEllipse(dotX - dotSize * 0.5f, dotY - dotSize * 0.5f, dotSize, dotSize);
}

void DispenserLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat();
    const auto buttonArea = bounds.reduced(2.0f);
    const auto isOn = button.getToggleState();

    const auto accentColor = juce::Colour(0xff4000c0);

    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(buttonArea, buttonArea.getHeight() * 0.5f);

    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawRoundedRectangle(buttonArea, buttonArea.getHeight() * 0.5f, 1.0f);

    constexpr float margin = 4.0f;
    const float circleSize = buttonArea.getHeight() - (margin * 2.0f);
    const float xPos = isOn ? (buttonArea.getRight() - circleSize - margin) : (buttonArea.getX() + margin);

    g.setColour(isOn ? accentColor : juce::Colours::whitesmoke.withAlpha(0.3f));
    g.fillEllipse(xPos, buttonArea.getY() + margin, circleSize, circleSize);

    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(juce::FontOptions(11.0f));

    const auto textRect = isOn
                              ? buttonArea.withTrimmedRight(circleSize + margin)
                              : buttonArea.withTrimmedLeft(circleSize + margin);

    g.drawText(button.getButtonText(), textRect, juce::Justification::centred, false);
}

juce::Label* DispenserLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = new SliderLabel();
    l->setJustificationType (juce::Justification::centred);
    return l;
}

void DispenserLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& ed)
{
}

void DispenserLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed)
{
}
