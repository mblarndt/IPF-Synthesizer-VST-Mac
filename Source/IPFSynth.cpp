#include "IPFSynth.h"

std::vector<float> IPFSynth::generateSineWaveTable()
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

std::vector<float> IPFSynth::generateSawtoothWaveTable()
{
    constexpr auto WAVETABLE_LENGTH = 64;
    std::vector<float> sawtoothWaveTable = std::vector<float>(WAVETABLE_LENGTH);

    for (auto i = 0; i < WAVETABLE_LENGTH; ++i)
    {
        sawtoothWaveTable[i] = -1.f + 2.f * static_cast<float>(i) / WAVETABLE_LENGTH;
    }

    return sawtoothWaveTable;
}

std::vector<float> IPFSynth::generateTriangleWaveTable()
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


std::vector<float> IPFSynth::generateSquareWaveTable()
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



void IPFSynth::initializeOscillators(juce::String chosenWavetable)
{
    oscillators.clear();
    constexpr auto OSCILLATOR_COUNT = 128;
    std::vector<float> waveTable;

    if(chosenWavetable == "sine")
        waveTable = generateSineWaveTable();
    else if(chosenWavetable == "square")
        waveTable = generateSquareWaveTable();
    else if (chosenWavetable == "saw")
        waveTable = generateSawtoothWaveTable();
    else if (chosenWavetable == "triangle")
        waveTable = generateTriangleWaveTable();
    else if (chosenWavetable == "custom")
        waveTable = getCustomWavetable();
    else
        waveTable = generateSineWaveTable();

    currentWavetable = waveTable;

    for (auto i = 0; i < OSCILLATOR_COUNT; ++i)
    {
        oscillators.emplace_back(waveTable, sampleRate);
    }
}

void IPFSynth::prepareToPlay(double sampleRate)
{
    this->sampleRate = sampleRate;

    initializeOscillators("sine");
}

void IPFSynth::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    auto currentSample = 0;

    for (const auto midiMetadata : midiMessages)
    {
        const auto message = midiMetadata.getMessage();
        const auto messagePosition = static_cast<int>(message.getTimeStamp());


        render(buffer, currentSample, messagePosition);
        currentSample = messagePosition;
        handleMidiEvent(message);
    }
    render(buffer, currentSample, buffer.getNumSamples());
}

void IPFSynth::setValues(float g, float alpha, float beta, float gamma)
{
    g_val = g;
    alpha_val = alpha;
    beta_val = beta;
    gamma_val = gamma;
}

void IPFSynth::setVolume(float newVolume)
{
    volume = newVolume;
}

void IPFSynth::setIPFRate(WavetableOscillator &oscilator, float rate)
{
    oscilator.ipf_rate = rate;
}


void IPFSynth::handleMidiEvent(const juce::MidiMessage& midiMessage)
{
    if (midiMessage.isNoteOn())
    {
        
        const auto oscillatorId = midiMessage.getNoteNumber();
        oscillators[oscillatorId].resetIPF();
        const auto frequency = midiMessage.getMidiNoteInHertz(oscillatorId);
        oscillators[oscillatorId].setFrequency(frequency);
        //const auto velocity = midiMessage.getVelocity();
        //velocity_mapped = juce::jmap<float>(velocity, 0.3, 0.8);
        //oscillators[oscillatorId].setAlpha(alpha_val);
        
    }
    else if (midiMessage.isNoteOff())
    {
        const auto oscillatorId = midiMessage.getNoteNumber();
        
        oscillators[oscillatorId].stop();
        //oscillators[oscillatorId].setAmplitude(1);
    }
    else if (midiMessage.isAllNotesOff())
    {
        for (auto& oscillator : oscillators)
        {
            oscillator.stop();
        }
    }
    
        
}

void IPFSynth::render(juce::AudioBuffer<float>& buffer, int beginSample, int endSample)
{
    auto* firstChannel = buffer.getWritePointer(0);
    for (auto& oscillator : oscillators)
    {
        oscillator.setG(g_val);
        oscillator.setAlpha(alpha_val);
        oscillator.setBeta(beta_val);
        oscillator.setGamma(gamma_val);
        oscillator.setphaseMod(phaseMod);
        setIPFRate(oscillator, ipf_rate);

        if (oscillator.isPlaying())
        {
            oscillator.startIndex = oscillator.index;
            for (auto sample = beginSample; sample < endSample; ++sample)
            {
                auto sam = oscillator.getSample();
                firstChannel[sample] += sam;
            }
        }
    }

    for (int channel = 1; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.applyGain(volume);
        auto* channelData = buffer.getWritePointer(channel);
        std::copy(firstChannel + beginSample, firstChannel + endSample, channelData + beginSample);
    }
}

std::vector<float> IPFSynth::getCustomWavetable()
{
    std::vector<float> wavetable = customWavetable;
    return wavetable;
}
