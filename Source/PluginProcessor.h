/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class TailMeasureAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    TailMeasureAudioProcessor();
    ~TailMeasureAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS();
//    void startTest();
    void initializeTest();
    juce::AudioParameterBool& getParamThing();
    juce::AudioProcessorValueTreeState parameters;
    
protected:
    
    std::atomic<float>* performingTest = nullptr;
private:
    juce::Random random;
    
    // Test parameters
    int cycleLengthInMils = 10;
    int cycleLengthInSamples;
    int cycleIndex = 0;
    int buildupInMils = 0.0f; // Could also be parameter. Determines how long white noise should be played before starting the measurement. Should be longer for reverbs that take some time to build up.
    const int intervalSeconds = 1; // Set the interval in seconds
    
    // Util parameters
    bool broadcastHasOccurred = false;
    bool testing = false;
    float minValue = std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::min();
    bool initialAmplitudeHasBeenMeasured = false;
    float initialAmplitude = 0.0f;
    float dBReduction = 60; // We mainly want RT60, but maybe this can also be a parameter if we are feeling fancy.
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TailMeasureAudioProcessor)
};
