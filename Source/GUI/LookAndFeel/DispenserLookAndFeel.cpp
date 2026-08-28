#include "DispenserLookAndFeel.h"

DispenserLookAndFeel::DispenserLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff101416));
    setColour(juce::Label::textColourId, juce::Colour(0xffd9e2df));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffd9e2df));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff1b2425));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff344345));
}

void DispenserLookAndFeel::drawRotarySlider(juce::Graphics& g, const int x, const int y, const int width, const int height,
                                            const float sliderPosProportional, const float rotaryStartAngle,
                                            const float rotaryEndAngle, juce::Slider& slider)
{
    const auto size = juce::jmin(width, height) - 10.0f;
    auto centerX = x + width * 0.5f;
    auto centerY = y + height * 0.5f;
    const auto ringBounds = juce::Rectangle(size, size).withCentre({centerX, centerY});

    constexpr float lineThickness = 6.0f;
    const float radius = (size - lineThickness) * 0.5f;
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    juce::Path track;
    track.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff293536));
    g.strokePath(
        track, juce::PathStrokeType(lineThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour(juce::Colour(0xffd5a85b));
    g.strokePath(valueArc, juce::PathStrokeType(lineThickness, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    g.setColour(juce::Colour(0xff182021));
    g.fillEllipse(ringBounds.reduced(lineThickness + 2.0f));

    g.setColour(slider.isMouseOverOrDragging() ? juce::Colour(0xfff0c875) : juce::Colour(0xffe4b45f));
    juce::Path pointer;
    constexpr float pW = 6.0f;
    const float pH = size * 0.15f;
    pointer.addRoundedRectangle(-pW * 0.5f, -radius + 2.0f, pW, pH, 2.0f);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    g.fillPath(pointer);
}

void DispenserLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsHighlighted);
    juce::ignoreUnused(shouldDrawButtonAsDown);

    const auto bounds = button.getLocalBounds().toFloat();
    const auto buttonArea = bounds.reduced(2.0f);
    const auto isOn = button.getToggleState();

    g.setColour(juce::Colour(0xff293536));
    g.fillRoundedRectangle(buttonArea, buttonArea.getHeight() * 0.5f);

    const float circleSize = buttonArea.getHeight() - 8.0f;
    const float xPos = isOn ? (buttonArea.getRight() - circleSize - 4.0f) : (buttonArea.getX() + 4.0f);

    g.setColour(isOn ? juce::Colour(0xffd5a85b) : juce::Colour(0xff101416));
    g.fillEllipse(xPos, buttonArea.getY() + 4.0f, circleSize, circleSize);

    g.setColour(juce::Colour(0xffd9e2df));
    const float fontSize = juce::jmin(14.0f, buttonArea.getHeight() * 0.5f);
    g.setFont(juce::FontOptions(fontSize));
    const auto textRect = isOn
                              ? buttonArea.withTrimmedRight(circleSize + 4.0f)
                              : buttonArea.withTrimmedLeft(circleSize + 4.0f);
    g.drawText(button.getButtonText(), textRect, juce::Justification::centred, false);
}
