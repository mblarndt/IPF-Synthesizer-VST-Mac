#include <cmath>
#include <vector>
#include "JuceHeader.h"
#include "WavetableOscillator.h"
#include <random>

WavetableOscillator::WavetableOscillator(std::vector<float> waveTable, double sampleRate)
    : waveTable{ std::move(waveTable) },
    sampleRate{ sampleRate }
{}

float WavetableOscillator::getSample()
{
    jassert(isPlaying());
    const auto wavetableSize = static_cast<float>(waveTable.size());

    // Original-Sample generieren
    index = index + (waveTable.size());
    index = std::fmod(index, wavetableSize);
    auto originalSample = interpolateLinearly();
    index += indexIncrement;

    ++sampleCounter;
    //desiredPeriodCount = desiredPeriodCount * 0.5;
    if (sampleCounter >= (desiredPeriodCount * ipf_rate))
    {
        calculate_amp();
        sampleCounter = 0;
        
    }
    
    auto sample = originalSample;
    
    if(phaseMod == true) {
        // Phasenverschobenes Sample generieren
        auto phaseShiftedSample = interpolateLinearlyWithPhaseShift(phaseShift);
        // Kombinierte Samples
        sample = originalSample + phaseShiftedSample;
    }
    
    sample *= amplitude;

    return sample;
}

float WavetableOscillator::interpolateLinearly() const
{
    const auto truncatedIndex = static_cast<size_t>(index);
    const auto nextIndex = static_cast<size_t>(std::ceil(index)) % waveTable.size();
    const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
    return waveTable[nextIndex] * nextIndexWeight +
        (1.f - nextIndexWeight) * waveTable[truncatedIndex];
}

float WavetableOscillator::interpolateLinearlyWithPhaseShift(float phaseOffset) const
{
    const auto phaseShiftedIndex = std::fmod(index + (waveTable.size() * phaseOffset), waveTable.size());
    const auto truncatedIndex = static_cast<size_t>(phaseShiftedIndex);
    const auto nextIndex = static_cast<size_t>(std::ceil(phaseShiftedIndex)) % waveTable.size();
    const auto nextIndexWeight = phaseShiftedIndex - static_cast<float>(truncatedIndex);
    return waveTable[nextIndex] * nextIndexWeight +
        (1.f - nextIndexWeight) * waveTable[truncatedIndex];
}




void WavetableOscillator::setPhaseShift(float shift)
{
    //Nimmt Werte von -1 bis 1
    if (shift <= 0)
        phaseShift = 1 - abs(shift);
    else
        phaseShift = shift;
    DBG(phaseShift);
}


void WavetableOscillator::setFrequency(float frequency)
{
    indexIncrement = frequency * static_cast<float>(waveTable.size())
        / static_cast<float>(sampleRate);
    desiredPeriodCount = round(sampleRate / frequency);
}

void WavetableOscillator::stop()
{
    index = 0.f;
    indexIncrement = 0.f;
}

bool WavetableOscillator::isPlaying() const
{
    return indexIncrement != 0.f;
}

void WavetableOscillator::setAmplitude(float amp)
{
    amplitude = amp;
}

void WavetableOscillator::setphaseMod(bool state)
{
    phaseMod = state;
}

void WavetableOscillator::resetIPF()
{
    g = g_init;
    g_pre = g_init;
    g_pre_2 = g_init;
    g_plus = 0;
    g_delta = 0;
    amplitude = 0;
    ipf_counter = 0;
}

float WavetableOscillator::ipf(float alpha, float beta, float gamma, float g, float g_pre, float g_pre_2)
{
    float g_pl;
    switch(ipf_counter) {

        case 0:
            g_pl = g - log( (1 / alpha) * (g));
        case 1:
            g_pl = g - log( (1 / alpha) * (g - (beta * exp(g - g_pre))));
        default:
            g_pl = g - log( (1 / alpha) * (g - (beta * exp(g - g_pre) + gamma * exp(g - g_pre_2) )  )  );
    }
    ipf_counter++;
    
    return g_pl;
}

float WavetableOscillator::calculate_amp()
{
    g_plus = ipf(alpha, beta, gamma, g, g_pre, g_pre_2);
    g_pre_2 = g_pre;
    g_pre = g;
    g = g_plus;

    g_delta = abs(g - g_pre);
    g_delta = remap(g_delta , alpha, beta, gamma);
    //DBG(g_delta);
    setPhaseShift(g_delta);
    
    const float g_plus_mapped = remap(g_plus, alpha, beta, gamma);
    //const float g_plus_fmod = juce::jmap<float>(g_plus_mapped, 0, 360);
    
    
    amplitude = std::abs(g_plus_mapped);

    return amplitude;
}

void WavetableOscillator::setG(float newG)
{
    g_init = newG;
}

void WavetableOscillator::setAlpha(float newAlpha)
{
    alpha = newAlpha / 100;
    //DBG("Alpha" + String(alpha));
}

void WavetableOscillator::setBeta(float newBeta)
{
    beta = (newBeta / 100);
    //DBG("Beta" + String(beta));
}

void WavetableOscillator::setGamma(float newGamma)
{
    gamma = (newGamma / 100);
    //DBG("Gamma" + String(gamma));
}

float WavetableOscillator::remap(float source, float alpha, float beta, float gamma)
{
    const float gs = alpha + beta + gamma;
    const float gdiff = g_init - gs;
    float g_min = gs - gdiff;
    float g_max = g_init;
    const float min_new = -1.0f;
    const float max_new = 1.0f;

    if(g_min == g_max)
        g_min = g_max - 0.001;
    const float out = juce::jmap<float>(source, g_min, g_max, min_new, max_new);

    return out;
}
