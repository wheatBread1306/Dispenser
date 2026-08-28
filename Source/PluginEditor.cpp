/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DispenserAudioProcessorEditor::DispenserAudioProcessorEditor (DispenserAudioProcessor& p)
  : AudioProcessorEditor (&p), audioProcessor (p), panel (p)
{
  setLookAndFeel(&lookAndFeel);
  addAndMakeVisible(panel);
  setSize(760, 500);
}

DispenserAudioProcessorEditor::~DispenserAudioProcessorEditor()
{
  setLookAndFeel(nullptr);
}

//==============================================================================
void DispenserAudioProcessorEditor::paint (juce::Graphics& g)
{
  g.fillAll(lookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
}

void DispenserAudioProcessorEditor::resized()
{
  panel.setBounds(getLocalBounds());
}
