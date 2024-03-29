#pragma once
#include <vector>
#include "HelperFunctions.h"

class WavetableOscillator
{
public:
    WavetableOscillator(std::vector<float> waveTable, double sampleRate);
    WavetableOscillator(const WavetableOscillator&) = delete;
    WavetableOscillator& operator=(const WavetableOscillator&) = delete;
    WavetableOscillator(WavetableOscillator&&) = default;
    WavetableOscillator& operator=(WavetableOscillator&&) = default;

    float getSample();
    void setPhaseShift(float shift);
    void setFrequency(float frequency);
    void stop();
    bool isPlaying() const;
    void setAmplitude(float amp);

    //IPF Methods
    void resetIPF();
    float ipf(float alpha, float beta, float gamma, float g, float g_pre, float g_pre_2);
    float calculate_amp();
    int ipf_counter;

    //Control Methods
    void setG(float newG);
    void setAlpha(float newAlpha);
    void setBeta(float newBeta);
    void setGamma(float newGamma);
    
    void setphaseMod(bool state, float value);
    void setampMod(bool state, float value);
    void setfreqMod(bool state, float value);

    bool phaseMod;
    bool ampMod;
    bool freqMod;
    bool fixedG;
    float ampmod;
    float phasemod;
    float freqmod;




    float startIndex = 0.f;
    float index = 0.f;
    float amplitude = 1;
    float g_init;
    float g = 1;
    float g_pre = 1;
    float g_pre_2 = 1;
    float g_plus;
    float g_delta;
    float g_degree;
    float g_rad;
    float phaseShift;
    float sampleCounter = 0;
    float desiredPeriodCount;

    float ipf_rate;



private:
    float interpolateLinearly() const;
    float interpolateLinearlyWithPhaseShift(float phaseOffset) const;
    float remap(float source, float alpha, float beta, float gamma);

    float alpha = 0.5;
    float beta = 0.49;
    float gamma = 0.3f;
    
    float indexIncrement = 0.f;
    std::vector<float> waveTable;
    double sampleRate;
    
    float currentFrequency;
    float fadeCounter = 0;
    
    HelperFunctions helper;
    dsp::Compressor<float> compressor;
};
