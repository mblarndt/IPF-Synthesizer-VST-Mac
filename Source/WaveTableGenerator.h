/*
  ==============================================================================

    WaveTableGenerator.h
    Created: 30 Jun 2023 1:37:24pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace std;

class WaveTableGenerator
{
public:
    vector<float> generateWaveTable(juce::String type, float phase = 0);

    vector<float> generateSineWaveTable(float phase = 0);

    vector<float> generateSawtoothWaveTable(float phase = 0);

    vector<float> generateTriangleWaveTable(float phase = 0);

    vector<float> generateSquareWaveTable(float phase = 0);
    
    vector<float> generateSinusWave(double amplitude = 1, double phase = 0);
};
