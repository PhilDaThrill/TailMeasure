/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TailMeasureAudioProcessor::TailMeasureAudioProcessor():

#ifndef JucePlugin_PreferredChannelConfigurations
AudioProcessor (BusesProperties()
     #if ! JucePlugin_IsMidiEffect
      #if ! JucePlugin_IsSynth
       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
      #endif
       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
     #endif
       ),
parameters (*this, nullptr, juce::Identifier ("APVTS"),
            {
    std::make_unique<juce::AudioParameterBool> (juce::ParameterID("performingTest", 1.0), // parameterID
                                                "performingTest",     // parameter name
                                                false)              // default value
            })
#endif
{
    
    performingTest = parameters.getRawParameterValue ("performingTest");
    
}

TailMeasureAudioProcessor::~TailMeasureAudioProcessor()
{
}

//==============================================================================
const juce::String TailMeasureAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TailMeasureAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TailMeasureAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TailMeasureAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TailMeasureAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TailMeasureAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TailMeasureAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TailMeasureAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TailMeasureAudioProcessor::getProgramName (int index)
{
    return {};
}

void TailMeasureAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TailMeasureAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    cycleLengthInSamples = cycleLengthInMils * sampleRate /1000;
}

void TailMeasureAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TailMeasureAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TailMeasureAudioProcessor::initializeTest()
{
    // Set up stuff like starttime i think
    startTime = std::chrono::steady_clock::now();
    broadcastHasOccurred = false;
}

void TailMeasureAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
//    auto totalNumInputChannels  = getTotalNumInputChannels();
        
    // Check input
    // Generate output audio
    auto* audioData = buffer.getWritePointer (0); // For the measurement, we only look at the left channel and ignore the other one(s).
    auto* audioDataR = buffer.getWritePointer (1); // However, we still need to broadcast stereo noise for a fair test.
    
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        
        auto& dataPoint = audioData[i];
        auto& dataPointR = audioDataR[i];
        if (testing)
        {
//            std::cout << dataPoint << std::endl;
            if (dataPoint < minValue) {
                minValue = dataPoint;
            }
            if (dataPoint > maxValue) {
                maxValue = dataPoint;
            }
            
            if (cycleIndex < cycleLengthInSamples)
            {
                cycleIndex++;
            } else
            {
                if (!initialAmplitudeHasBeenMeasured)
                {
                    initialAmplitude = maxValue - minValue;
                    initialAmplitudeHasBeenMeasured = true;
                    std::cout << "Initial amplitude has been measured." << std::endl;
                } else
                {
                    auto currentAmplitude = maxValue - minValue;
                    if (currentAmplitude < 0.1 * initialAmplitude) // TODO: !!! This is blatantly wrong. Adapt this to conform to the -60 dB thing.
                    {
                        auto currentTime = std::chrono::steady_clock::now();
                        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                        std::cout << "RT 60 of " << elapsedSeconds << " seconds!" << std::endl;
                        testing = false;
                        initialAmplitudeHasBeenMeasured = false;
                    }
                }
                cycleIndex = 0;
                minValue = std::numeric_limits<float>::max();
                maxValue = std::numeric_limits<float>::min();
            }
            
            audioData[i] = 0.0f; // We need to block the output to avoid feedback.
            audioDataR[i] = 0.0f;
            // Output something
            //
            //        auto currentTime = std::chrono::steady_clock::now();
            //        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            //
            //        if (elapsedSeconds >= intervalSeconds) {
            //            std::cout << "Min value: " << minValue << " - Max value: " << maxValue << std::endl;
            //            startTime = currentTime; // Reset the start time
            //        }
        } else {
            auto broadCasting = *performingTest < 0.5f ? false : true;
            if (broadCasting)
            {
                dataPoint = random.nextFloat() * 0.25f - 0.125f; // Overwrite incoming data with white noise
                dataPointR = random.nextFloat() * 0.25f - 0.125f; // Overwrite incoming data with white noise
                broadcastHasOccurred = true;
                // Maybe include a mechanism to automatically stop broadcast instead of leaving it to the user
                // Maybe give the option of "manual broadcasting" via togglebutton?
            } else
            {
                if (broadcastHasOccurred)
                {
                    // Launch testing process
                    initializeTest();
                    testing = true;
                }
                
            }
            audioData[i] = 0.0f; // We need to block the output to avoid feedback.
            audioDataR[i] = 0.0f;
        }
    }
}

//==============================================================================
bool TailMeasureAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TailMeasureAudioProcessor::createEditor()
{
    return new TailMeasureAudioProcessorEditor (*this);
}

juce::AudioProcessorValueTreeState& TailMeasureAudioProcessor::getAPVTS()
{
    return parameters;
}

//==============================================================================
void TailMeasureAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TailMeasureAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TailMeasureAudioProcessor();
}

//void TailMeasureAudioProcessor::startTest()
//{
//    bool currentValue = static_cast<bool>(*performingTest);
//    *performingTest = !currentValue;
//    
//    std::cout << "Hurrr: " << *performingTest << std::endl;
//    std::cout << "Durrrr: " << static_cast<bool>(*performingTest) << std::endl;
//}

