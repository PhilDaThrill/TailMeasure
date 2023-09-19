/*
  ==============================================================================

    MainComponent.h
    Created: 24 Mar 2023 5:02:52pm
    Author:  Philip Rüchardt

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"


//==============================================================================
/*
*/
class MainComponent  : public juce::Component, public juce::Button::Listener, public TailMeasureAudioProcessor
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
    juce::ToggleButton& getStartTestButton();
    

private:
    
    juce::ToggleButton startTestButton {};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
