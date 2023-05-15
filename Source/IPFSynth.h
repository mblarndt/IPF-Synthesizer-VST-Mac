#pragma once
#include <JuceHeader.h>

#include "WavetableOscillator.h"

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

    float g_val;
    float alpha_val;
    float beta_val;
    float gamma_val;
    
    float velocity_mapped;

    std::vector<float> currentWavetable;
    std::vector<float> customWavetable;

private:
    static std::vector<float> generateSineWaveTable();
    static std::vector<float> generateSquareWaveTable();
    static std::vector<float> generateSawtoothWaveTable();
    static std::vector<float> generateTriangleWaveTable();

    
    void handleMidiEvent(const juce::MidiMessage& midiMessage);
    void render(juce::AudioBuffer<float>& buffer, int beginSample, int endSample);

    std::vector<float> getCustomWavetable();

    double sampleRate;

    float frequency;

    std::vector<WavetableOscillator> oscillators;
};
