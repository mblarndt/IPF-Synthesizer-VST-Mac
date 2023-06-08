/*
  ==============================================================================

    ArrayPlotComponent.cpp
    Created: 8 Jun 2023 1:04:02pm
    Author:  Ben

  ==============================================================================
*/

#include <juce_gui_basics/juce_gui_basics.h>
#include "JuceHeader.h"

class ArrayPlotComponent : public juce::Component
{
public:
    ArrayPlotComponent(const std::vector<float>& data)
        : dataArray(data)
    {
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::white); // Hintergrundfarbe

        g.setColour(juce::Colours::black);
        g.drawRect(getLocalBounds(), 1); // Rahmen um das Diagramm

        // Datenlinie zeichnen
        g.setColour(juce::Colours::red);
        for (int i = 0; i < dataArray.size() - 1; ++i)
        {
            float x1 = juce::mapToLog10(i, 0, dataArray.size() - 1, 0, getWidth());
            float x2 = juce::mapToLog10(i + 1, 0, dataArray.size() - 1, 0, getWidth());
            float y1 = juce::mapFromLog10(dataArray[i], 0, 1, getHeight(), 0);
            float y2 = juce::mapFromLog10(dataArray[i + 1], 0, 1, getHeight(), 0);
            g.drawLine(x1, y1, x2, y2);
        }
    }

private:
    std::vector<float> dataArray;
};

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        // Beispiel-Daten erzeugen
        std::vector<float> data = { 0.1f, 0.5f, 0.8f, 0.3f, 0.6f, 0.2f, 0.9f };

        // ArrayPlotComponent hinzufügen und positionieren
        arrayPlotComponent = new ArrayPlotComponent(data);
        addAndMakeVisible(arrayPlotComponent);
        arrayPlotComponent->setBounds(getLocalBounds());
    }

    void resized() override
    {
        arrayPlotComponent->setBounds(getLocalBounds());
    }

private:
    ArrayPlotComponent* arrayPlotComponent;
};

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow()
        : juce::DocumentWindow("Array Plot", juce::Colours::lightgrey, juce::DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new MainComponent(), true);

        centreWithSize(getWidth(), getHeight());
        setVisible(true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};