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
#include "WavetableGenerator.h"
#include <complex.h>
#include "HelperFunctions.h"
#include <random>

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
    TextButton button_help;
    ToggleButton toggle_bypass;
    ToggleButton toggle_phasemod;
    ToggleButton toggle_freqmod;
    ToggleButton toggle_ampmod;
    ToggleButton toggle_exp;
    ToggleButton toggle_fixstate;
    
    TextButton radioButton_13_sine;
    TextButton radioButton_13_square;
    TextButton radioButton_13_triangle;
    TextButton radioButton_13_saw;
    TextButton radioButton_14_ipf;
    TextButton radioButton_14_signal;
    
    ComboBox modeMenu;
    ComboBox thresholdMenu;
    
    Slider slider_input;
    Slider slider_gain;

    Slider slider_phasemod;
    Slider slider_fmod;
    Slider slider_ampmod;
    Slider slider_exp;
    Slider dial_g;
    Slider dial_rate;

    Slider dial_alpha;
    Slider dial_beta;
    Slider dial_gamma;

    Slider dial_phasemod;
    Slider dial_ampmod;
    Slider dial_freqmod;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> alphaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> betaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gammaAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampmodAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> phasemodAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqmodAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ampmodToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freqmodToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> phasemodToggleAttachment;
    
    Path waveTablePath;

    String fileName;

    bool plot_IPF;
    bool plotSignal;
    String previousBtn;
    bool setupDone;
    bool showHelp;
    
    juce::Image helpBox;

    std::unique_ptr<FileChooser> chooser;

    std::vector<float> waveTable;

    juce::Array<juce::ToggleButton*> toggleButtons;
    juce::Array<juce::TextButton*> textButtons;
    juce::Array<juce::Slider*> sliders;
    juce::Array<juce::TextButton*> radioButtons;


    class csvVectors {
    public:
        std::vector<float> g0;
        std::vector<float> alpha;
        std::vector<float> beta;
        std::vector<float> gamma;
        std::vector<float> behaviour;
        std::vector<float> percent;
    };
    
    csvVectors fixedMode;
    csvVectors stableMode;
    csvVectors bifMode;
    csvVectors chaosMode;
    
    csvVectors threshold100Mode;
    csvVectors threshold90Mode;
    csvVectors threshold80Mode;
    csvVectors threshold70Mode;
    

    Array<Colour> alphaColours;
    Array<Colour> betaColours;
    Array<Colour> gammaColours;

    Colour shape_colour;

    
    // Erstellen Sie die y-Daten
    std::vector<std::vector<float>> yData = { {} };
    std::vector<std::vector<float>> xData = { {} };

    CustomLookAndFeel claf;
    WaveTableGenerator wtg;
    HelperFunctions helper;
    

    void dial_init(juce::Slider& name, Slider::SliderStyle style, float initValue ,int min = 0, int max = 100, float steps = 0.01);
    void button_init(juce::TextButton& name, String& button_text);
    void radioButton_init(juce::TextButton& button, String& button_text, int id);
    void toggle_init(juce::ToggleButton& name);
    void label_init(juce::Label& name, String& text);
    void dropdown_init(juce::ComboBox& comboBox, const StringArray& items, int defaultIndex);
    void paint_text(juce::Graphics& graphics, const juce::String& font, float size, Colour colour, const juce::String& text, int x, int y, bool centred = true);
    void paint_shape(juce::Graphics& graphics, Rectangle<int> bounds, Colour colour, bool dropShadow = true);
    void paint_shadow(juce::Graphics& graphics, Rectangle<int> bounds, Colour colour = Colour::fromRGBA(0, 0, 0, 100), int radius = 10, Point<int> offset = Point<int>(3, 3));
    void paint_label(juce::Graphics& graphics, Slider& name, String title);
    void sliderValueChanged(juce::Slider* slider);
    void buttonValueChanged(juce::Button* button);
    void dropdownValueChanged(juce::ComboBox* comboBox);
    void openFileAsync();
    void adjustSliders();
    void updateWaveTablePath();
    void resetRange(juce::Slider& name, String ctrlID);
    
    void updateShownList(String mode, String threshold = "None");
    
    std::vector<float> arange(float start, float stop, float step = 1.0);
    std::vector<float> calculateIPF(float gVal, float alphaVal, float betaVal, float gammaVal, bool calcSignal = false);


    void drawColorfulCircle(Graphics& g, int centerX, int centerY, int diameter, const Array<Colour>& colours);

    std::vector<float> readCSVFromString(const std::string& dataString, csvVectors* list);

    std::pair<std::vector<float>, std::vector<float>> getAlphaPercentage(float targetBeta, float targetGamma);
    std::pair<std::vector<float>, std::vector<float>> getBetaPercentage(float targetAlpha, float targetGamma);
    std::pair<std::vector<float>, std::vector<float>> getGammaPercentage(float targetAlpha, float targetBeta);

    Array<Colour> generateColors(const std::vector<float>& behaviour, const std::vector<float>& percentage);

    void updateCircleColors();

    float roundToTwoDecimalPlaces(float value);
    
    void reloadIPF();
    
    float linearInterpolation(float x, float x0, float y0, float x1, float y1);
    
    std::vector<float> interpolateArray(const std::vector<float>& inputArray, int newLength);



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IPFSynthesizerVSTAudioProcessorEditor)
};

