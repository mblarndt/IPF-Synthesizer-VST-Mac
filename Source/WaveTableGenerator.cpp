/*
  ==============================================================================

    WaveTableGenerator.cpp
    Created: 30 Jun 2023 1:37:25pm
    Author:  Ben

  ==============================================================================
*/

#include "WaveTableGenerator.h"

vector<float> WaveTableGenerator::generateWaveTable(juce::String type, float phase)
{
    vector<float> waveTable;

    if (type == "sine")
        waveTable = generateSineWaveTable(phase);
    else if (type == "saw")
        waveTable = generateSawtoothWaveTable(phase);
    else if (type == "triangle")
        waveTable = generateTriangleWaveTable(phase);
    else if (type == "square")
        waveTable = generateSquareWaveTable(phase);

    return waveTable;
}

vector<float> WaveTableGenerator::generateSineWaveTable(float phase)
{
    constexpr auto WAVETABLE_LENGTH = 64;
    const auto PI = std::atanf(1.f) * 4;
    std::vector<float> sineWaveTable = std::vector<float>(WAVETABLE_LENGTH);

    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        sineWaveTable[i] = std::sinf(2 * PI * static_cast<float>(i) / WAVETABLE_LENGTH);
    }

    return sineWaveTable;
}

std::vector<float> WaveTableGenerator::generateSawtoothWaveTable(float phase)
{
    constexpr auto WAVETABLE_LENGTH = 64;
    std::vector<float> sawtoothWaveTable = std::vector<float>(WAVETABLE_LENGTH);

    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        sawtoothWaveTable[i] = -1.f + 2.f * static_cast<float>(i) / WAVETABLE_LENGTH;
    }

    return sawtoothWaveTable;
}

std::vector<float> WaveTableGenerator::generateTriangleWaveTable(float phase)
{
    constexpr auto WAVETABLE_LENGTH = 64;
    const auto PI = std::atanf(1.f) * 4;
    std::vector<float> triangleWaveTable = std::vector<float>(WAVETABLE_LENGTH);

    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        float value = (2.0f * i / WAVETABLE_LENGTH) - 1.0f;
        triangleWaveTable[i] = (2.0f / PI) * std::asin(std::sinf(PI * value));
    }

    return triangleWaveTable;
}


std::vector<float> WaveTableGenerator::generateSquareWaveTable(float phase)
{
    constexpr auto WAVETABLE_LENGTH = 64;
    std::vector<float> squareWaveTable = std::vector<float>(WAVETABLE_LENGTH);
    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        if (i < WAVETABLE_LENGTH / 2)
            squareWaveTable[i] = 1.0f;
        else
            squareWaveTable[i] = -1.0f;
    }

    return squareWaveTable;

}

std::vector<float> WaveTableGenerator::generateSinusWave(double amplitude, double phase) {
    const int numSamples = 64;
    const auto PI = std::atanf(1.f) * 4;
    std::vector<float> waveform;

    for (int i = 0; i < numSamples; ++i) {
        double t = static_cast<double>(i) / numSamples; // Time in the range [0, 1]
        float value = static_cast<float>(amplitude * sin(2 * PI * t + phase));
        waveform.push_back(value);
    }

    return waveform;
}
