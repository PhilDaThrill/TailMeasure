/*
  ==============================================================================

    MainComponent.cpp
    Created: 24 Mar 2023 5:02:52pm
    Author:  Philip Rüchardt

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"



//==============================================================================
MainComponent::MainComponent()
{
    startTestButton.setButtonText(juce::String("Start Test"));
    
    addAndMakeVisible(startTestButton);
    startTestButton.setClickingTogglesState (true);
    setSize(400, 300);
    setVisible(true);

}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void MainComponent::resized()
{
    auto lb = getLocalBounds();
    
    auto topMargin = lb.getHeight()*0.05;
    
    auto xTestButtons = lb.getWidth() * 0.5;
    auto widthButtons = lb.getWidth() * 0.23;
    auto heightButtons = lb.getHeight() * 0.1;
    auto gapButtons = lb.getHeight() * 0.02; // There was this juce::limit thingy that could make sense here
    
    startTestButton.setBounds(xTestButtons, topMargin + 0 * (heightButtons + gapButtons), widthButtons, heightButtons);
    
}

juce::TextButton& MainComponent::getStartTestButton()
{
    return startTestButton;
}

