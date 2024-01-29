/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "IPFSynth.h"

//==============================================================================
/**
*/
class IPFSynthesizerVSTAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    IPFSynthesizerVSTAudioProcessor();
    ~IPFSynthesizerVSTAudioProcessor() override;
    
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

    std::vector<float> getWavetable();

    //==============================================================================
    float g;
    float alpha;
    float beta;
    float alphaFine;

    float ipf_rate;
    float volume;
    float velocity_mapped;
    
    bool phaseMod;
    bool ampMod;
    bool freqMod;
    bool fixedG;
    float ampmod;
    float phasemod;
    float freqmod;
    
    int randomCounter;

    juce::String chosenWavetable;
    juce::String oldWavetable;

    std::vector<float> customWavetable;
    juce::AudioProcessorValueTreeState apvts;

    void addSliderParameter(String id, String name, NormalisableRange<float> range, float initialValue);
    void addToggleParameter(String id, String name, bool initialValue);

private:
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IPFSynthesizerVSTAudioProcessor)
    IPFSynth synth;
    dsp::Compressor<float> compressor;
};
