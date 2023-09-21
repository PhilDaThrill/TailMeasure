/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TailMeasureAudioProcessorEditor::TailMeasureAudioProcessorEditor (TailMeasureAudioProcessor& p)
    : AudioProcessorEditor (&p),
    audioProcessor (p),
    valueTreeState(p.getAPVTS()),
    mainComponent(valueTreeState)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    centreWithSize (mainComponent.getWidth(), mainComponent.getHeight());
    addAndMakeVisible(mainComponent);
}

TailMeasureAudioProcessorEditor::~TailMeasureAudioProcessorEditor()
{
}

//==============================================================================
void TailMeasureAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void TailMeasureAudioProcessorEditor::resized()
{
    
}
