#pragma once
#include <JuceHeader.h>

#include "WavetableOscillator.h"
#include "WaveTableGenerator.h"

class IPFSynth
{
public:
    void prepareToPlay(double sampleRate);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);
    void setValues(float g, float alpha, float beta, float gamma);
    void setVolume(float volume);
    void setIPFRate(WavetableOscillator &oscilator, float rate);
    void initializeOscillators(juce::String chosenWavetable);

    float volume;
    float ipf_rate;
    bool phaseMod;
    bool ampMod;
    bool freqMod;
    bool fixedG;

    float g_val;
    float alpha_val;
    float beta_val;
    float gamma_val;
    float ampmod;
    float phasemod;
    float freqmod;
    
    float velocity_mapped;

    WaveTableGenerator generator;
    std::vector<float> currentWavetable;
    std::vector<float> customWavetable;

private:    
    void handleMidiEvent(const juce::MidiMessage& midiMessage);
    void render(juce::AudioBuffer<float>& buffer, int beginSample, int endSample);

    std::vector<float> getCustomWavetable();

    double sampleRate;

    float frequency;

    std::vector<WavetableOscillator> oscillators;
};
