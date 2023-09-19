/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TailMeasureAudioProcessorEditor::TailMeasureAudioProcessorEditor (TailMeasureAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(p.getAPVTS())
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    setResizable(true, true);
    addAndMakeVisible(mainComponent);
    
    startTestAttachment.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (valueTreeState, "performingTest", mainComponent.getStartTestButton()));
}

TailMeasureAudioProcessorEditor::~TailMeasureAudioProcessorEditor()
{
}

//==============================================================================
void TailMeasureAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void TailMeasureAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
