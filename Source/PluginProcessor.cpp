/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
IPFSynthesizerVSTAudioProcessor::IPFSynthesizerVSTAudioProcessor()
    : AudioProcessor (BusesProperties()
                      #if ! JucePlugin_IsMidiEffect
                       #if ! JucePlugin_IsSynth
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       #endif
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      #endif
                      ),
    apvts(*this, nullptr)
{
    // Erstelle und füge Parameter hinzu
    addSliderParameter("g", "State", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 100.0f);
    addSliderParameter("alpha", "Input Strength", NormalisableRange<float>(0.0f, 100.0f, 0.01f), 50.0f);
    addSliderParameter("beta", "1. Reflection", NormalisableRange<float>(0.0f, 100.0f, 0.01f), 45.0f);
    addSliderParameter("gamma", "2. Reflection", NormalisableRange<float>(0.0f, 100.0f, 0.01f), 4.0f);
    addSliderParameter("gain", "Gain", NormalisableRange<float>(-100.0f, 20.0f, 0.1f), -10.0f);
    addSliderParameter("rate", "Rate", NormalisableRange<float>(0, 4, 0.25f), 1.0f);
    
    juce::ValueTree defaultState("MyPluginState");  // Erstelle eine ValueTree-Instanz
    apvts.state = defaultState;  // Weise die ValueTree-Instanz dem apvts-Objekt zu
}




IPFSynthesizerVSTAudioProcessor::~IPFSynthesizerVSTAudioProcessor()
{
}

//==============================================================================
const juce::String IPFSynthesizerVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool IPFSynthesizerVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool IPFSynthesizerVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool IPFSynthesizerVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double IPFSynthesizerVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int IPFSynthesizerVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int IPFSynthesizerVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void IPFSynthesizerVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String IPFSynthesizerVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void IPFSynthesizerVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void IPFSynthesizerVSTAudioProcessor::prepareToPlay (double sampleRate, int)
{
    
    synth.setValues(1, 50, 45, 5);
    synth.setVolume(-10);
    synth.prepareToPlay(sampleRate);
}

void IPFSynthesizerVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool IPFSynthesizerVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void IPFSynthesizerVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    synth.setValues(g, alpha, beta, gamma);
    synth.setVolume(volume);
    synth.ipf_rate = ipf_rate;
    synth.phaseMod = phaseMod;

    if (chosenWavetable != oldWavetable) {
        if (chosenWavetable == "custom")
            synth.customWavetable = customWavetable;

        synth.initializeOscillators(chosenWavetable);
        oldWavetable = chosenWavetable;
    }

    for (auto i = 0; i < buffer.getNumChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    synth.processBlock(buffer, midiMessages);

}

//==============================================================================
bool IPFSynthesizerVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* IPFSynthesizerVSTAudioProcessor::createEditor()
{
    return new IPFSynthesizerVSTAudioProcessorEditor (*this);
}

//==============================================================================
void IPFSynthesizerVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void IPFSynthesizerVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IPFSynthesizerVSTAudioProcessor();
}



std::vector<float> IPFSynthesizerVSTAudioProcessor::getWavetable() {
    return synth.currentWavetable;
}

void IPFSynthesizerVSTAudioProcessor::addSliderParameter(String id, String name, NormalisableRange<float> range, float initialValue)
{
    auto parameter = std::make_unique<juce::AudioParameterFloat>(
        ParameterID(id, 1),
        name,
        range,
        initialValue
    );
    apvts.createAndAddParameter(std::move(parameter));
}