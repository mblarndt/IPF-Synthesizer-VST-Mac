/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"
#include "IPFSynth.h"
#include "WaveTablePlot.h" 
#include "cmp_plot.h"
#include "cmp_frame.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace juce;

//==============================================================================
/**
*/
class IPFSynthesizerVSTAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    IPFSynthesizerVSTAudioProcessorEditor(IPFSynthesizerVSTAudioProcessor&);
    ~IPFSynthesizerVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    cmp::Plot plot;



private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    IPFSynthesizerVSTAudioProcessor& audioProcessor;

    TextButton button_file;
    ToggleButton toggle_bypass;
    ToggleButton toggle_gdelta;
    ToggleButton toggle_fmod;
    ToggleButton toggle_exp;
    TextButton radioButton_13_sine;
    TextButton radioButton_13_square;
    TextButton radioButton_13_triangle;
    TextButton radioButton_13_saw;
    TextButton radioButton_14_ipf;
    TextButton radioButton_14_signal;
    Slider slider_input;
    Slider slider_gain;
    Slider slider_gdelta;
    Slider slider_fmod;
    Slider slider_exp;
    Slider dial_g;
    Slider dial_rate;

    Slider dial_alpha;
    Slider dial_beta;
    Slider dial_gamma;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> alphaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> betaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gammaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    
    Path waveTablePath;

    String fileName;

    bool plot_IPF;
    bool plotSignal;
    String previousBtn;
    bool setupDone;

    std::unique_ptr<FileChooser> chooser;

    std::vector<float> waveTable;

    juce::Array<juce::ToggleButton*> toggleButtons;
    juce::Array<juce::TextButton*> textButtons;
    juce::Array<juce::Slider*> sliders;
    juce::Array<juce::TextButton*> radioButtons;



    std::vector<float> csvAlpha;
    std::vector<float> csvBeta;
    std::vector<float> csvIterations;

    Array<Colour> betaColours;

    Colour shape_colour;

    
    // Erstellen Sie die y-Daten
    std::vector<std::vector<float>> yData = { {} };
    std::vector<std::vector<float>> xData = { {} };

    CustomLookAndFeel claf;

    void dial_init(juce::Slider& name, Slider::SliderStyle style, float initValue ,int min = 0, int max = 100, float steps = 0.01);
    void button_init(juce::TextButton& name, String& button_text);
    void radioButton_init(juce::TextButton& button, String& button_text, int id);
    void toggle_init(juce::ToggleButton& name);
    void label_init(juce::Label& name, String& text);
    void paint_text(juce::Graphics& graphics, const juce::String& font, float size, Colour colour, const juce::String& text, int x, int y, bool centred = true);
    void paint_shape(juce::Graphics& graphics, Rectangle<int> bounds, Colour colour, bool dropShadow = true);
    void paint_shadow(juce::Graphics& graphics, Rectangle<int> bounds, Colour colour = Colour::fromRGBA(0, 0, 0, 100), int radius = 10, Point<int> offset = Point<int>(3, 3));
    void paint_label(juce::Graphics& graphics, Slider& name, String title);
    void sliderValueChanged(juce::Slider* slider);
    void buttonValueChanged(juce::Button* button);
    void openFileAsync();
    void adjustSliders();
    void updateWaveTablePath();
    void resetRange(juce::Slider& name, String ctrlID);
    std::vector<float> arange(float start, float stop, float step = 1.0);
    
    std::vector<float> calculateIPF(float gVal, float alphaVal, float betaVal, float gammaVal, bool calcSignal = false);

    std::vector<float> generateSineWaveTable();

    void drawColorfulCircle(Graphics& g, int centerX, int centerY, int diameter, const Array<Colour>& colours);

    std::vector<float> readCSVFromString(const std::string& dataString);

    std::vector<float> getIterationsForAlpha(float targetAlpha);

    Array<Colour> generateColors(const std::vector<float>& iterations);

    double roundToTwoDecimalPlaces(double value);



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IPFSynthesizerVSTAudioProcessorEditor)
};

