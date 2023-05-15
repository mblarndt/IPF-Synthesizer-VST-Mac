/*
  ==============================================================================

    WaveTablePlot.h
    Created: 2 May 2023 12:28:18pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

class WaveTablePlotComponent : public Component
{
public:
    WaveTablePlotComponent()
    {
        setSize(200, 200);
        bg = Rectangle<int>(200, 30, 200, 200);
        setBounds(bg);
    }

    void paint(Graphics& g) override
    {

        //g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

        g.setColour(Colours::white);
        g.strokePath(waveTablePath, PathStrokeType(1.f));

        g.setColour(Colours::red);
        g.drawLine(0, bg.getHeight() / 2, bg.getWidth(), bg.getHeight() / 2, 2.f);
    }

    void resized() override
    {
        updateWaveTablePath();
    }

    std::vector<float> waveTable;

private:
    int x, y, w, h;
    Path waveTablePath;
    Rectangle<int> bg;
    

    void updateWaveTablePath()
    {
        waveTablePath.clear();

        const float xScale = getWidth() / static_cast<float>(waveTable.size() - 1);
        const float yScale = getHeight() / 2.f;

        waveTablePath.startNewSubPath(0.f, getHeight() / 2.f);

        for (size_t i = 0; i < waveTable.size(); ++i)
        {
            const float x = i * xScale;
            const float y = waveTable[i] * yScale + getHeight() / 2.f;
            waveTablePath.lineTo(x, y);
        }
    }
};
