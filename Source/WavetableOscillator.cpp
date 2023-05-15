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
    index = index + (waveTable.size() * 0);
    index = std::fmod(index, wavetableSize);
    
    auto sample = interpolateLinearly();
    index += indexIncrement;


    ++periodCounter;
    //desiredPeriodCount = desiredPeriodCount * 0.5;
    if (periodCounter >= (desiredPeriodCount * ipf_rate))
    {
        calculate_amp();
        periodCounter = 0;
    }

    sample *= amplitude;

    return sample;
}

void WavetableOscillator::setPhaseShift(float shift)
{
    phaseShift = shift;
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

void WavetableOscillator::resetIPF()
{
    g = g_init;
    g_pre = g_init;
    g_pre_2 = g_init;
    g_plus = 0;
    g_delta = 0;
    amplitude = 0;
}

float WavetableOscillator::interpolateLinearly() const
{
    const auto truncatedIndex = static_cast<size_t>(index);
    const auto nextIndex = static_cast<size_t>(std::ceil(index)) % waveTable.size();
    const auto nextIndexWeight = index - static_cast<float>(truncatedIndex);
    return waveTable[nextIndex] * nextIndexWeight +
        (1.f - nextIndexWeight) * waveTable[truncatedIndex];
}

float WavetableOscillator::ipf(float alpha, float beta, float gamma, float g, float g_pre, float g_pre_2)
{
    float g_pl = g - log( (1 / alpha) * (g - (beta * exp(g - g_pre) + gamma * exp(g - g_pre_2) )  )  );
    //float g_pl = g - log((1 / alpha) * (g - (beta * exp(g - g_pre))));
    DBG(beta);
    return g_pl;
}

float WavetableOscillator::calculate_amp()
{

    

    g_plus = ipf(alpha, beta, gamma, g, g_pre, g_pre_2);
    g_pre_2 = g_pre;
    g_pre = g;
    g = g_plus;

    g_delta = abs(g - g_pre);
    g_delta = remap(g_delta, alpha, beta, gamma);

    setPhaseShift(abs(g_delta));


    const float g_plus_mapped = remap(g_plus, alpha, beta, gamma);

    
    
    amplitude = std::abs(g_plus_mapped);

    return amplitude;
}

void WavetableOscillator::setG(float newG)
{
    g_init = newG;
}

void WavetableOscillator::setAlpha(float newAlpha)
{
    alpha = newAlpha;
}

void WavetableOscillator::setBeta(float newBeta)
{
    beta = newBeta;
}

void WavetableOscillator::setGamma(float newGamma)
{
    gamma = newGamma;
}

float WavetableOscillator::remap(float source, float alpha, float beta, float gamma)
{
    const float g_init = 1.f;
    const float gs = alpha + beta + gamma;
    const float gdiff = g_init - gs;
    const float g_min = gs - gdiff - 0;
    const float g_max = g_init + 0;
    const float min_new = -1.0f;
    const float max_new = 1.0f;

    const float out = juce::jmap<float>(source, g_min, g_max, min_new, max_new);

    return out;
}
