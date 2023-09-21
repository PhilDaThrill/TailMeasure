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
class MainComponent  : public juce::Component, public juce::Button::Listener, public TailMeasureAudioProcessor,
public juce::Timer
{
public:
    MainComponent(std::shared_ptr<juce::AudioProcessorValueTreeState> params);
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    juce::TextButton& getStartTestButton();
    
    juce::Label statusLabel;
    juce::Label rtText;
    
private:
    juce::TextButton startTestButton {};
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    
    std::shared_ptr<juce::AudioProcessorValueTreeState> params;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
