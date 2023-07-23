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
    vector<float> generateWaveTable(juce::String type);

    vector<float> generateSineWaveTable(float phaseshift);

    vector<float> generateSawtoothWaveTable();

    vector<float> generateTriangleWaveTable();

    vector<float> generateSquareWaveTable();
};
