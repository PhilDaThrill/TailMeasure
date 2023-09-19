/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "MainComponent.h"

//==============================================================================
/**
*/
class TailMeasureAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TailMeasureAudioProcessorEditor (TailMeasureAudioProcessor&);
    ~TailMeasureAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TailMeasureAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    MainComponent mainComponent;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> startTestAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TailMeasureAudioProcessorEditor)
};
