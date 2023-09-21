/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TailMeasureAudioProcessor::TailMeasureAudioProcessor():
AudioProcessor (BusesProperties()
       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
       ),
statuses("Idle", "Testing", "RT60"),
parameters (std::make_shared<juce::AudioProcessorValueTreeState>(*this,
                                                                 nullptr,
                                                                 juce::Identifier ("Parameters"),
                                                                 createParameterLayout()
                                                                 ))
{
    
    performingTest = parameters->getRawParameterValue ("performingTest");
    rt60Value = parameters->getRawParameterValue ("rt60Value");
    statusParam = parameters->getRawParameterValue ("status");
    
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

    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::createLCR()
        )
    {
        return false;
    }
    
    return true;
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
                    initialAmplitude = std::max(0.1f, maxValue - minValue); // 0.1 in case there is no tail to speak of at all
                    initialAmplitudeHasBeenMeasured = true;
                    std::cout << "Initial amplitude has been measured." << std::endl;
                } else
                {
                    auto currentAmplitude = maxValue - minValue;
                    
                    auto threshold = initialAmplitude / std::pow(10, dBReduction / 20.0);
                    
                    if (currentAmplitude < threshold)
                    {
                        auto currentTime = std::chrono::steady_clock::now();
                        float elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
                        float elapsedSecondsWithHundredths = static_cast<float>(elapsedMilliseconds) / 1000.0f;
                        *statusParam = 3.0f; // Notify GUI that RT 60 has been measured
                        *rt60Value = elapsedSecondsWithHundredths;
                        std::cout << "RT 60 of " << elapsedSecondsWithHundredths << " seconds!" << std::endl;
                        testing = false;
                        initialAmplitudeHasBeenMeasured = false;
                    }
                }
                cycleIndex = 0;
                minValue = std::numeric_limits<float>::max();
                maxValue = std::numeric_limits<float>::min();
            }
            
            dataPoint = 0.0f; // We need to block the output to avoid feedback.
            dataPointR = 0.0f;
            
        } else {
            auto testHasBeenStarted = *performingTest < 0.5f ? false : true;
            
            if (testHasBeenStarted || broadcastHasOccurred)
            {
                if (testHasBeenStarted)
                {
                    broadcasting = true;
                    *statusParam = 1.0f; // Alert GUI that broadcsting has begun
                    startTime = std::chrono::steady_clock::now(); // Measure starttime for 1 second broadcast of noise
                    *performingTest = 0.0f;
                }else if (broadcastHasOccurred)
                {
                    // Launch testing process
                    initializeTest();
                    testing = true;
                    
                }
                dataPoint = 0.0f; // Feedback prevention
                dataPointR = 0.0f;
                
            }else if (broadcasting)
            {
                dataPoint = random.nextFloat() * 0.25f - 0.125f; // Overwrite incoming data with white noise
                dataPointR = random.nextFloat() * 0.25f - 0.125f; // Overwrite incoming data with white noise
                
                auto currentTime = std::chrono::steady_clock::now();
                float elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
                if (elapsedMilliseconds / 1000.0f > broadcastTimeInSeconds) {
                    broadcasting = false;
                    broadcastHasOccurred = true;
                    *statusParam = 2.0f; // Notify GUI that test phase has begun
                }
            } else {
                dataPoint = 0.0f; // Feedback prevention
                dataPointR = 0.0f;
            }
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

std::shared_ptr<juce::AudioProcessorValueTreeState> TailMeasureAudioProcessor::getAPVTS()
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

juce::AudioProcessorValueTreeState::ParameterLayout TailMeasureAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    params.add(std::make_unique<juce::AudioParameterBool> (juce::ParameterID("performingTest", 1.0), // parameterID
                                                           "performingTest",     // parameter name
                                                           false
                                                           ));              // default value
    params.add(std::make_unique<juce::AudioParameterFloat> (
                                                            juce::ParameterID("rt60Value", 1.0), // parameterID
                                                            "rt60Value",     // parameter name
                                                            juce::NormalisableRange<float> (0.0f, 100000.0f, 0.01f),
                                                            0.0f
                                                            ));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(
                                                            juce::ParameterID("status", 1.0),
                                                            "status",
                                                           juce::NormalisableRange<float> (0.0f, 2.0f, 1.0f),
                                                            0.0f
                                                            ));
    return params;
}


