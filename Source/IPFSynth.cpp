#include "IPFSynth.h"


void IPFSynth::initializeOscillators(juce::String chosenWavetable)
{
    oscillators.clear();
    constexpr auto OSCILLATOR_COUNT = 128;
    std::vector<float> waveTable;

    if (chosenWavetable == "custom")
        waveTable = getCustomWavetable();
    else
        waveTable = generator.generateWaveTable(chosenWavetable);

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
    if(fixedG)
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
    oscilator.ipfRate = rate;
}


void IPFSynth::handleMidiEvent(const juce::MidiMessage& midiMessage)
{
    if (midiMessage.isNoteOn())
    {
        
        const auto oscillatorId = midiMessage.getNoteNumber();
        oscillators[oscillatorId].resetIPF();
        const auto frequency = midiMessage.getMidiNoteInHertz(oscillatorId);
        oscillators[oscillatorId].setFrequency(frequency);
        oscillators[oscillatorId].setampMod(ampMod, ampmod);
        oscillators[oscillatorId].setphaseMod(ampMod, phasemod);
        oscillators[oscillatorId].setfreqMod(ampMod, freqmod);
    
        
        if(fixedG == false) {
            float velocity = midiMessage.getVelocity();
            float gs = alpha_val + beta_val + gamma_val;
            
            if(velocity <= 64) {
                float g0 = gs - (gs * (velocity / 64));
                g_val = g0;
                oscillators[oscillatorId].gInit = g0;
            }
            else {
                float g0 = gs + (gs * ((velocity-64) / 64));
                g_val = g0;
                oscillators[oscillatorId].gInit = g0;
            }
        }
        
        

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
        oscillator.setAlphaFine(gamma_val);
        oscillator.setphaseMod(phaseMod, phasemod);
        oscillator.setampMod(ampMod, ampmod);
        oscillator.setfreqMod(freqMod, freqmod);
        oscillator.fixedG = fixedG;

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
