#include <cmath>
#include <vector>
#include "JuceHeader.h"
#include "WavetableOscillator.h"
#include <random>

WavetableOscillator::WavetableOscillator(std::vector<float> waveTable, double sampleRate)
    : waveTable{ std::move(waveTable) },
    sampleRate{ sampleRate }
{
    gDelta = 0;
}

float WavetableOscillator::getSample()
{
    jassert(isPlaying());
    const auto wavetableSize = static_cast<float>(waveTable.size());

    // Original-Sample generieren
    index = index + (waveTable.size());
    index = std::fmod(index, wavetableSize);
    auto originalSample = interpolateLinearly();
    
    
    if(freqMod == true) {
        //float frequency = currentFrequency +  100 * g_delta * freqmod;
        float frequency = 1/((1 / currentFrequency) * (1+(gDelta * freqmod)));
        //float frequency = 1 / ((1 / currentFrequency) * abs(g / g_plus));
        
        indexIncrement = frequency  * static_cast<float>(waveTable.size()) / static_cast<float>(sampleRate);
        //desiredPeriodCount = round(sampleRate / (1 / currentFrequency) * (1 + g_delta * freqmod));
        desiredPeriodCount = round(sampleRate / frequency);
    }
    
    
    index += indexIncrement;
    
    ++sampleCounter;
    //desiredPeriodCount = desiredPeriodCount * 0.5;
    
    //Clock to trigger IPF
    if (sampleCounter >= (desiredPeriodCount))
    {
        calculate_amp();
        sampleCounter = 0;
    }
    
    auto sample = originalSample;
    
    if(phaseMod == true) {
        // Phasenverschobenes Sample generieren
        auto phaseShiftedSample = interpolateLinearlyWithPhaseShift(phaseShift * phasemod);
        // Kombinierte Samples
        sample = (originalSample / 2) + (phaseShiftedSample / 2);
    }
    
    /*
    if(fadeCounter < 1000) {
        sample = sample * (fadeCounter / 1000);
        fadeCounter = fadeCounter + 1;
    }
    */
    
    if (ampMod == true)
        sample *= amplitude;
    
    
    //Prüfen ob Zahl komplex ist
    if (!std::isfinite(sample)) {
        return 0;
    }
     
    
    float compressed_sample = helper.compressSample(sample);
    //compressor.processSample(1, sample);


    return compressed_sample;
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
    //DBG(phaseShift);
     
}


void WavetableOscillator::setFrequency(float frequency)
{
    currentFrequency = frequency;
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

void WavetableOscillator::setphaseMod(bool state, float value)
{
    phaseMod = state;
    phasemod = value;
}

void WavetableOscillator::setampMod(bool state, float value)
{
    ampMod = state;
    ampmod = value;
}

void WavetableOscillator::setfreqMod(bool state, float value)
{
    freqMod = state;
    freqmod = value;
}

void WavetableOscillator::resetIPF()
{
    g = gInit;
    gPre = gInit;
    gPlus = 0;
    gDelta = 0;
    amplitude = 0;
    ipf_counter = 0;
    fadeCounter = 0;
}

float WavetableOscillator::ipf(float alpha, float beta, float alphaFine, float g, float g_pre)
{
    DBG(alpha);
    float g_pl;
    
    float lAlpha = alpha + alphaFine;
    
    
    if(ipf_counter == 0) {
        float logArg = (1 / lAlpha) * (g);
        if(logArg <= 0)
            logArg = 0.5;
        g_pl = g - log(logArg);
    }
    else {
        float logArg = (1 / lAlpha) * (g - (beta * exp(g - g_pre) )  );
        if(logArg <= 0)
            logArg = 0.5;
        g_pl = g - log(logArg);
    }
    
    ipf_counter++;

    return g_pl;
}

float WavetableOscillator::calculate_amp()
{
    gPlus = ipf(alpha, beta, alphaFine, g, gPre);
    
    gPre = g;
    
    //const float g_mapped = remap(g, alpha, beta, gamma);
    float gs = alpha + beta + alphaFine;
    float g_mapped = (g / gs) -  1;
    const float g_plus_mapped = (gPlus / gs) - 1;
    
    gDelta = abs(gPlus - g);
    
    g = gPlus;
    
    
    //setPhaseShift(g_delta);
    phaseShift = helper.calculatePhaseshift(gDelta);
    
    float signalmod = g_plus_mapped * ampmod;
    amplitude = (signalmod + 1) * gs;

    return amplitude;
}

void WavetableOscillator::setG(float newG)
{
    gInit = newG;
}

void WavetableOscillator::setAlpha(float newAlpha)
{
    alpha = round(newAlpha*100)/100;
    //DBG("Alpha" + String(alpha));
}

void WavetableOscillator::setBeta(float newBeta)
{
    beta = round(newBeta*100)/100;
    //DBG("Beta" + String(beta));
}

void WavetableOscillator::setAlphaFine(float newAlphaFine)
{
    alphaFine = newAlphaFine*0.001;
    //DBG("Gamma" + String(gamma));
}

float WavetableOscillator::remap(float source, float alpha, float beta, float alphaFine)
{
    const float gs = alpha + beta + alphaFine;
    const float gdiff = gInit - gs;
    float g_min = gs - gdiff;
    float g_max = gInit;
    const float min_new = -1.0f;
    const float max_new = 1.0f;

    if(g_min == g_max)
        g_min = g_max - 0.001;
    const float out = juce::jmap<float>(source, g_min, g_max, min_new, max_new);

    return out;
}
