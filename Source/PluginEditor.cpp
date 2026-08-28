/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DispenserAudioProcessorEditor::DispenserAudioProcessorEditor(DispenserAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), panel(p)
{
    setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(panel);
    setSize(550, 400);
}

DispenserAudioProcessorEditor::~DispenserAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void DispenserAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(lookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));

    const auto bounds = getLocalBounds().toFloat();

    g.fillAll(juce::Colours::black);

    const juce::ColourGradient purpleGrad(
        juce::Colours::purple.withAlpha(0.8f), bounds.getBottomLeft(),
        juce::Colours::transparentBlack, bounds.getCentre(),
        true); // true = radial
    g.setGradientFill(purpleGrad);
    g.fillAll();

    const juce::ColourGradient blueGrad(
        juce::Colours::blue.withAlpha(0.8f), bounds.getTopRight(),
        juce::Colours::transparentBlack, bounds.getTopLeft(),
        true);
    g.setGradientFill(blueGrad);
    g.fillAll();
}

void DispenserAudioProcessorEditor::resized()
{
    panel.setBounds(getLocalBounds());
}
