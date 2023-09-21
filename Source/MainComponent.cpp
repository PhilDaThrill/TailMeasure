/*
  ==============================================================================

    MainComponent.cpp
    Created: 24 Mar 2023 5:02:52pm
    Author:  Philip Rüchardt

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"

#include <format>


//==============================================================================
MainComponent::MainComponent(std::shared_ptr<juce::AudioProcessorValueTreeState> params_c):
params(params_c)
{
    startTimerHz(10);
    
    startTestButton.setButtonText(juce::String("Start Test"));
    
    addAndMakeVisible(startTestButton);
    startTestButton.addListener(this);
    setSize(300, 100);
    setVisible(true);

    addAndMakeVisible (statusLabel);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    statusLabel.setText("Ready", juce::dontSendNotification);
    statusLabel.setFont(juce::Font(18));
    statusLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible (rtText);
    rtText.setText ("by Orpheus Effects", juce::dontSendNotification);
    
    rtText.setFont(juce::Font(10));
    rtText.setColour (juce::Label::textColourId, juce::Colours::black);
    rtText.setJustificationType (juce::Justification::bottom);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);   // clear the background
    g.drawRoundedRectangle(statusLabel.getX(), statusLabel.getY(), statusLabel.getWidth(), statusLabel.getHeight(), statusLabel.getHeight()/10, 1);
}

void MainComponent::resized()
{
    auto lb = getLocalBounds();
    
    double relativeTopMargin = 0.15;
    auto topMargin = lb.getHeight()*relativeTopMargin;
    
    auto leftGapButtons = topMargin;
    auto widthButtons = lb.getWidth() * 0.4;
    auto heightButtons = lb.getHeight() * (1.0 - 2*relativeTopMargin);
    
    auto gapBetweenElements = lb.getHeight() * 0.2; // There was this juce::limit thingy that could make sense here
    
    auto xStatusLabel = leftGapButtons + widthButtons + gapBetweenElements;
    auto yStatusLabel = (lb.getHeight() / 2.0) - 0.5 * heightButtons;
    auto widthStatusLabel = lb.getWidth() - xStatusLabel - leftGapButtons;
    
    if (juce::approximatelyEqual(params->getRawParameterValue ("status")->load(), 1.0f))
    {
        statusLabel.setText("Broadcasting noise...", juce::dontSendNotification); // TODO: Add varying number of dots lol
    } else if (juce::approximatelyEqual(params->getRawParameterValue ("status")->load(), 2.0f))
    {
        statusLabel.setText("Listening...", juce::dontSendNotification); // TODO: Add varying number of dots lol
    } else if(juce::approximatelyEqual(params->getRawParameterValue ("status")->load(), 3.0f))
    {
        float number = params->getRawParameterValue("rt60Value")->load() + 0.005f; // Adding .005 to achieve rounding behavior
        std::string num_text = std::to_string(number);
        std::string rounded = num_text.substr(0, num_text.find(".")+3); // String handling in cpp really is a drag
        std::string text = "RT60: " + rounded + " sec";
        statusLabel.setText(text, juce::dontSendNotification);
    }
    
    startTestButton.setBounds(leftGapButtons, topMargin, widthButtons, heightButtons / 2);
    rtText.setBounds(leftGapButtons, topMargin + heightButtons / 2, widthButtons, heightButtons / 2);
    
    statusLabel.setBounds(xStatusLabel, yStatusLabel, widthStatusLabel, heightButtons);
    
}

juce::TextButton& MainComponent::getStartTestButton()
{
    return startTestButton;
}

void MainComponent::timerCallback()
{
    resized();
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &startTestButton) {
        *params->getRawParameterValue("performingTest") = 1.0f; // Initiate testing procedure
    }
}

