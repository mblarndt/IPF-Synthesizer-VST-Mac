/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"


using namespace juce;



//==============================================================================
IPFSynthesizerVSTAudioProcessorEditor::IPFSynthesizerVSTAudioProcessorEditor(IPFSynthesizerVSTAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    // Erstelle den AudioProcessorValueTreeState-Objekt
    setupDone = false;

    toggle_init(toggle_bypass);
    toggle_bypass.setToggleState(true, NotificationType::sendNotification);
    toggle_init(toggle_gdelta);
    toggle_init(toggle_fmod);
    
    String rb1 = String("sine");
    String rb2 = String("square");
    String rb3 = String("triangle");
    String rb4 = String("saw");
    
    radioButton_init(radioButton_13_sine, rb1, int(13));
    radioButton_13_sine.setToggleState(true, dontSendNotification);
    radioButton_init(radioButton_13_square, rb2, int(13));
    radioButton_init(radioButton_13_triangle, rb3, int(13));
    radioButton_init(radioButton_13_saw, rb4, int(13));


    
    String fileText = String("Select File");
    button_init(button_file, fileText);
    
    //Gain Slider Setup
    dial_init(slider_gain, Slider::SliderStyle::LinearBarVertical, 0);
    resetRange(slider_gain, "gain");
    slider_gain.setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
    slider_gain.setNumDecimalPlacesToDisplay(0);

    dial_init(dial_g, Slider::SliderStyle::Rotary, 1, 0, 1, 0.01);
    
    dial_init(dial_alpha, Slider::SliderStyle::Rotary, 50);
    dial_alpha.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(1, 215, 50));

    dial_init(dial_beta, Slider::SliderStyle::Rotary, 43);
    dial_init(dial_gamma, Slider::SliderStyle::Rotary, 3);

    dial_init(dial_rate, Slider::SliderStyle::Rotary, 1);

    slider_input.setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal); // Zwei Werte, horizontale Ausrichtung
    slider_input.setRange(0.0, 100.0, 1.0); // Minimaler Wert, Maximaler Wert, Schrittgröße
    slider_input.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 15);
    //slider_input.setValue(2; // Anfangswerte für den minimalen und maximalen Wert
    //addAndMakeVisible(&slider_input);
    
    gAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "g", dial_g);
    alphaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "alpha", dial_alpha);
    betaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "beta", dial_beta);
    gammaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gamma", dial_gamma);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", slider_gain);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "rate", dial_rate);

    addAndMakeVisible(plot);

    String rb5 = String("IPF");
    String rb6 = String("Signal");
    radioButton_init(radioButton_14_ipf, rb5, int(14));
    radioButton_init(radioButton_14_signal, rb6, int(14));
    radioButton_14_ipf.setToggleState(true, dontSendNotification);
    
    
    //plot.xLim(0, 500);
    plotSignal = false;
    yData[0] = calculateIPF(1, dial_alpha.getValue() / 100, dial_beta.getValue() / 100, dial_gamma.getValue() / 100, plotSignal);
    plot.yLim(-1.2, 1.2);
    plot.xLim(0, 100);
    plot.plot(yData);


    
    setSize(800, 479);
    
    setLookAndFeel(&claf);


    const char* txtData = reinterpret_cast<const char*>(BinaryData::daten_csv);
    readCSVFromString(txtData);
    std::vector<float> iters = getIterationsForAlpha(0.5);
    DBG(iters.size());

    setupDone = true;

}

IPFSynthesizerVSTAudioProcessorEditor::~IPFSynthesizerVSTAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void IPFSynthesizerVSTAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillAll(Colour::fromRGB(58, 58, 58));

    g.setColour(juce::Colours::white);
    
    shape_colour = Colour::fromRGB(58, 58, 58);
    
    //Right Side - Volume Field
    paint_shape(g, Rectangle<int>(695, 40, 105, 438), shape_colour);
    
    //Down Side - IPF Field
    paint_shape(g, Rectangle<int>(0, 334, 508+ 187, 145), shape_colour);
    
    //Middle Field
    paint_shape(g, Rectangle<int>(0, 48, 508+187, 286), shape_colour);
    
    //Mod Field
    paint_shape(g, Rectangle<int>(0, 249, 186, 85), shape_colour);
    
    //Load Wave Field
    paint_shape(g, Rectangle<int>(0, 165, 186, 84), shape_colour);
    
    //Wavetable Field
    paint_shape(g, Rectangle<int>(0, 48, 186, 117), shape_colour);
    
    //Header Field
    paint_shape(g, Rectangle<int>(0, 0, 800, 48), shape_colour);
    

    String font = String("Arial");
    Colour text_colour = Colour::fromRGB(255, 255, 255);
    
    paint_label(g,dial_g, "State");
    paint_label(g,dial_rate, "Rate");
    paint_label(g,dial_alpha, "Input Strength");
    paint_label(g,dial_beta, "1. Reflection");
    paint_label(g,dial_gamma, "2. Reflection");
    paint_text(g, font, 18, text_colour, String("Gain"), 745.5, 440);
    
    paint_text(g, font, 18, text_colour, String("Wavetable"), 93, 58 + 18);
    paint_text(g, font, 18, text_colour, String("Load Wave"), 93, 174 + 18);
    paint_text(g, font, 18, text_colour, String("Phase-Mod"), 30, 267 + 18, false);
    paint_text(g, font, 18, text_colour, String("f-Mod"), 30, 302 + 18, false);
    paint_text(g, font, 18, text_colour, String("MBLA    |   IPF - Synthesizer"), 40.0, 15+ 18, false);

    // Größe und Position des Kreises

    int diameter = 65;
    // Zeichne den Kreis mit den Bereichen
    drawColorfulCircle(g, 490, 406, diameter, betaColours);

    

    
    //setAlpha(audioProcessor.velocity_mapped);

    //g.setColour(Colours::blue);
    //g.strokePath(waveTablePath, PathStrokeType(2.f));
    
}

void IPFSynthesizerVSTAudioProcessorEditor::resized()
{
    int margin = 140;

    toggle_bypass.setBounds(750, 14, 23, 23);
    toggle_gdelta.setBounds(132, 266, 23, 23);
    toggle_fmod.setBounds(132, 301, 23, 23);

    button_file.setBounds(36, 205, 114, 30);

    radioButton_13_sine.setBounds(14.37, 87, 79.04, 27.54);
    radioButton_13_square.setBounds(92.21, 87, 79.04, 27.54);
    radioButton_13_triangle.setBounds(92.21, 114, 79.04, 27.54);
    radioButton_13_saw.setBounds(14.37, 114, 79.04, 27.54);

    radioButton_14_ipf.setBounds(528, 70, 75, 25);
    radioButton_14_signal.setBounds(603, 70, 75, 25);

    
    slider_gain.setBounds(729, 92.21, 37.12, 306.56);
    
    int startpos = 25;
    dial_rate.setBounds(startpos, 369.5, 90.0, 90.0);
    dial_g.setBounds(startpos + margin, 369.5, 90.0, 90.0);
    dial_alpha.setBounds(startpos + margin * 2, 369.5, 90.0, 90.0);
    dial_beta.setBounds(startpos + margin * 3, 369.5, 90.0, 90.0);
    dial_gamma.setBounds(startpos + margin * 4, 369.5, 90.0, 90.0);
    slider_input.setBounds(215, 184, 250, 20);

    plot.setBounds(185, 62, 500, 270);

    

}
void IPFSynthesizerVSTAudioProcessorEditor::dial_init(juce::Slider& name, Slider::SliderStyle style, float initValue,int min, int max, float steps) {
    sliders.add(&name);
    name.setSliderStyle(style);
    name.setRange(min, max, steps);
    name.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 15);
    name.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    name.setValue(initValue);
    name.onValueChange = [this, nameCopy = &name]() { sliderValueChanged(nameCopy); };
    addAndMakeVisible(&name);
}
void IPFSynthesizerVSTAudioProcessorEditor::button_init(juce::TextButton& name, String& button_text) {
    textButtons.add(&name);
    name.setButtonText(button_text);
    name.onClick = [this, nameCopy = &name]() { buttonValueChanged(nameCopy); };
    addAndMakeVisible(&name);
}
void IPFSynthesizerVSTAudioProcessorEditor::radioButton_init(juce::TextButton& button, String& button_text, int id) {
    radioButtons.add(&button);
    button.setButtonText(button_text);
    button.setRadioGroupId(id);
    button.setClickingTogglesState(true);
    button.setConnectedEdges(Button::ConnectedOnBottom + Button::ConnectedOnTop + Button::ConnectedOnLeft + Button::ConnectedOnRight);

    // F�ge den Button hinzu und setze seinen ActionListener
    button.onClick = [this, nameCopy = &button]() { buttonValueChanged(nameCopy); };
    addAndMakeVisible(&button);
}
void IPFSynthesizerVSTAudioProcessorEditor::toggle_init(juce::ToggleButton& name) {
    toggleButtons.add(&name);
    name.onStateChange = [this, nameCopy = &name]() { buttonValueChanged(nameCopy); };
    addAndMakeVisible(&name);
}
void IPFSynthesizerVSTAudioProcessorEditor::label_init(juce::Label& name, String& text) {
    name.setText(text, dontSendNotification);
    addAndMakeVisible(&name);
}
void IPFSynthesizerVSTAudioProcessorEditor::paint_text(juce::Graphics& graphics, const juce::String& font, float size, Colour colour, const juce::String& text, int x, int y, bool centred) {
    juce::Font myFont(font, size, juce::Font::plain);
    graphics.setColour(colour);
    graphics.setFont(myFont);
    
    juce::Rectangle<int> bounds(x, y - 18, myFont.getStringWidth(text), myFont.getHeight());
    
    if(centred == true)
        bounds.setX(x - bounds.getWidth() / 2);
    
    graphics.drawText(text, bounds, juce::Justification::centred, true);
}

void IPFSynthesizerVSTAudioProcessorEditor::paint_shape(juce::Graphics& graphics, Rectangle<int> bounds, Colour colour, bool dropShadow) {
    if(dropShadow == true)
        paint_shadow(graphics, bounds);
    graphics.setColour(colour);
    graphics.fillRect(bounds);
}

void IPFSynthesizerVSTAudioProcessorEditor::paint_shadow(juce::Graphics& graphics, Rectangle<int> bounds, Colour colour, int radius, Point<int> offset) {
    DropShadow shadow = DropShadow(colour, radius, offset);
    shadow.drawForRectangle(graphics, bounds);
}

void IPFSynthesizerVSTAudioProcessorEditor::paint_label(juce::Graphics& graphics, Slider& name, String title)
{
    String font = String("Arial");
    Colour text_colour = Colour::fromRGB(255, 255, 255);

    paint_text(graphics, font, 18, text_colour, title, name.getX() + (name.getWidth() / 2), name.getY());
}

void IPFSynthesizerVSTAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{

    if (slider == &dial_alpha)
    {
        float value = slider->getValue();
        if (value <= 0.0f)
            value = 0.00001f;

        audioProcessor.alpha = value;

        std::vector<float> iterations = getIterationsForAlpha(roundToTwoDecimalPlaces(value/100));
        betaColours = generateColors(iterations);

        float new_beta_max = value;
        dial_beta.setRange(0.0, new_beta_max, 0.01);

        // Regel: alpha > beta
        if (audioProcessor.beta >= new_beta_max)
            audioProcessor.beta = new_beta_max;

        

        dial_beta.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_beta_max, 0));

        float new_gamma_max = dial_beta.getValue();

        // Regel: beta + gamma <= alpha
        if (new_gamma_max >= (value - dial_beta.getValue()))
            new_gamma_max = value - dial_beta.getValue() - 0.001;
        if (new_gamma_max <= 0.0f)
            new_gamma_max = 0.00001f;

        dial_gamma.setRange(0.0, new_gamma_max, 0.01);

        // Regel: gamma <= new_gamma_max
        if (dial_gamma.getValue() >= new_gamma_max)
            audioProcessor.gamma = new_gamma_max;

        dial_gamma.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_gamma_max, 0));

        dial_beta.repaint();
        dial_gamma.repaint();
    }


    else if (slider == &dial_beta) {
        //plot.plot(yData);

        float value = slider->getValue();
        if (value <= 0.0f)
            value = 0.00001f;

        // Regel: alpha > beta
        if (value >= dial_alpha.getValue())
            value = audioProcessor.alpha - 0.00001f;

        audioProcessor.beta = value;

        float new_max = value;

        // Regel: gamma <= alpha - beta
        if (new_max > (dial_alpha.getValue() - value))
            new_max = dial_alpha.getValue() - value;

        if (new_max <= 0.0f)
            new_max = 0.00001f;

        dial_gamma.setRange(0.0, new_max, 0.01);

        // Regel: gamma <= new_max
        if (dial_gamma.getValue() >= new_max)
            audioProcessor.gamma = new_max - 0.001;

        dial_gamma.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_max, 0));

        dial_gamma.repaint();
    }
    else if (slider == &dial_gamma) {
        //plot.plot(yData);

        float value = slider->getValue();
        if (value <= 0.0f)
            value = 0.00001f;

        // Regel: beta > gamma
        if (value >= audioProcessor.beta)
            value = audioProcessor.beta - 0.001f;

        audioProcessor.gamma = value;
    }
    else if (slider == &dial_g) {
        audioProcessor.g = slider->getValue();
    }
    else if (slider == &slider_gain) {
        float volume = std::pow(10.0f, slider->getValue() / 20.0f);
        audioProcessor.volume = volume;
    }
    else if (slider == &dial_rate) {
        audioProcessor.ipf_rate = slider->getValue();
    }


    std::vector<float> yArray = yData[0];
    // Überprüfen, ob yData gültig ist
    bool isValidYData = std::all_of(yArray.begin(), yArray.end(), [](float value) {
        return std::isfinite(value);  // Überprüft, ob der Wert endlich und kein NaN oder Inf ist
        });

    if (!isValidYData) {
        // yData ist ungültig, erstelle ein Array mit Nullen
        yData[0] = std::vector<float>(50, 0.0f);
    }

    if (setupDone) {
        yData[0] = calculateIPF(dial_g.getValue( ), dial_alpha.getValue() / 100, dial_beta.getValue() / 100, dial_gamma.getValue() / 100, plotSignal);
        plot.plot(yData);
    }
}


void IPFSynthesizerVSTAudioProcessorEditor::buttonValueChanged(juce::Button* button)
{
    for (auto* name : toggleButtons)
    {
        // Handle slider changes here
        if (button == name)
        {
            //String btn = button->getButtonText();
            //float value = button->getToggleState();
            //DBG(btn + " changed to " + String(value));

        }
        else if (button == &button_file) {
            openFileAsync();
            for (auto* b : radioButtons) {
                b->setToggleState(true, NotificationType::dontSendNotification);
            }
                
        }
        else if (button == &toggle_gdelta) {
            bool value = button->getToggleState();
            audioProcessor.phaseMod = value;
        }
    }

    for (auto* name : textButtons)
    {
        // Handle slider changes here
        if (button == name)
        {
            String btn = button->getButtonText();
            DBG(btn + ": Button Click");
        }
    }

    for (auto* name : radioButtons)
    {
        // Handle slider changes here
        if (button == name)
        {
            if (button->getRadioGroupId() == 13) {
                String btn = button->getButtonText();
                DBG(btn + ": Button Click");
                audioProcessor.chosenWavetable = btn;
                updateWaveTablePath();
            }
            if (button->getRadioGroupId() == 14) {
                String btn = button->getButtonText();
                
                if (btn != previousBtn) {
                    DBG(btn + ": Button Click");

                    if (btn == "Signal") {
                        plotSignal = true;
                        plot.xLim(0, 6400);
                    }
                    else {
                        plotSignal = false;
                        plot.xLim(0, 100);

                    }
                    yData[0] = calculateIPF(1, dial_alpha.getValue() / 100, dial_beta.getValue() / 100, dial_gamma.getValue() / 100, plotSignal);
                    plot.plot(yData);
                }

                previousBtn = btn;

            }
        }
    }
}




void IPFSynthesizerVSTAudioProcessorEditor::updateWaveTablePath()
{
    waveTablePath.clear();
    waveTable = audioProcessor.getWavetable();

    
    const float xScale = getWidth() / static_cast<float>(waveTable.size() - 1);
    const float yScale = getHeight() / 2.f;
    waveTablePath.startNewSubPath(0.f, getHeight() / 2.f);
    for (size_t i = 0; i < waveTable.size(); ++i)
    {
        const float x = i * xScale;
        const float y = waveTable[i] * yScale + getHeight() / 2.f;
        waveTablePath.lineTo(x, y);
    }
   
    repaint();
}


void IPFSynthesizerVSTAudioProcessorEditor::openFileAsync()
{
    chooser = std::make_unique<FileChooser>("TEST", File::getSpecialLocation(File::userHomeDirectory), "*.wav*");
    chooser->launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
        [this](const FileChooser& fc)
        {
            File file = fc.getResult();
            fileName = file.getFileName();
            button_file.setButtonText(fileName);
            audioProcessor.chosenWavetable = "custom"; 

            AudioFormatManager formatManager;
            formatManager.registerBasicFormats();
            std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
            std::vector<float> samples;

            if (reader != nullptr)
            {
                AudioBuffer<float> buffer(reader->numChannels, reader->lengthInSamples);
                reader->read(&buffer, 0, reader->lengthInSamples, 0, true, true);

                // Hier kannst du auf die Daten im AudioBuffer zugreifen
                
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                {
                    float* channelData = buffer.getWritePointer(channel);
                    for (int i = 0; i < buffer.getNumSamples(); ++i)
                    {
                        float sample = channelData[i];
                        samples.push_back(sample);
                    }
                }
            }

            audioProcessor.customWavetable = samples;
        });
}

void IPFSynthesizerVSTAudioProcessorEditor::adjustSliders()
{
}

void IPFSynthesizerVSTAudioProcessorEditor::resetRange(juce::Slider &name, String ctrlID) {
    juce::NormalisableRange<float> parameterRange = audioProcessor.apvts.getParameterRange(ctrlID);
    name.setRange(parameterRange.start, parameterRange.end, parameterRange.interval);
}

std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::arange(float start, float stop, float step)
{
    std::vector<float> result;

    for (float value = start; value < stop; value += step) {
        result.push_back(value);
    }

    return result;
}

std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::calculateIPF(float gVal, float alphaVal, float betaVal, float gammaVal, bool calcSignal)
{
    float state = gVal;
    float g_pre = state;
    float g_pre_2 = state;
    float gs = alphaVal + betaVal + gammaVal;
    float gdiff = 1 - gs;
    float g_min = gs - gdiff;
    float g_max = state;


    std::vector<float> g_array;
    std::vector<float> g_interp;
    std::vector<float> modified_interp; // Neues Array für den modifizierten Output
    std::vector<float> output;

    float g_plus;

    int max_iterations = 100;  // Maximale Anzahl von Iterationen

    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        // Berechnung der nächsten Iteration des g-Werts
        g_plus = state - std::log((1 / alphaVal) * (state - (betaVal * std::exp(state - g_pre) + gammaVal * std::exp(state - g_pre_2))));
        g_pre_2 = g_pre;
        g_pre = state;
        state = g_plus;

        g_array.push_back(g_plus);
        float g_plus_inp = 2 * ((g_plus - g_min) / (g_max - g_min)) - 1;
        g_interp.push_back(g_plus_inp);

        // Überprüfung auf ungültigen Wert
        if (!std::isfinite(g_plus)) {
            g_interp.clear();
            g_interp.resize(max_iterations, 0.0f);
            break;
        }
    }

    // Falls weniger als 50 Werte berechnet wurden, mit Nullen auffüllen
    /*
    if (g_interp.size() < 50) {
        g_interp.resize(max_iterations, 0.0f);
    }
    */

    if (calcSignal == true) {
        
        std::vector<float> sine = generateSineWaveTable();

        for (int i = 0; i < g_interp.size(); ++i) {
            float value = g_interp[i];
            for (int p = 0; p < sine.size(); p++) {
                modified_interp.push_back(value * sine[p]);
            }

        }
    }
    
    if (calcSignal == false) {
        for (int i = max_iterations; i < 6400; ++i) {
            g_interp.push_back(0);
        }
        
        output = g_interp;
    }    
    else
        output = modified_interp;

    return output;
}


std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::generateSineWaveTable()
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

void IPFSynthesizerVSTAudioProcessorEditor::drawColorfulCircle(Graphics& g, int centerX, int centerY, int diameter, const Array<Colour>& colours)
{
    int startAngle = -150; // Startwinkel in Grad
    int endAngle = 150; // Endwinkel in Grad
    // Berechnung des Radius
    int radius = diameter / 2;

    // Berechnung des Winkels pro Bereich
    float anglePerSection = static_cast<float>(endAngle - startAngle) * float_Pi / 180.0f / colours.size();

    // Zeichne den Kreis mit den einzelnen Bereichen
    for (int i = 0; i < colours.size(); ++i)
    {
        g.setColour(colours[i]);

        // Erstelle den Pfad für den aktuellen Bereich
        Path sectionPath;
        sectionPath.addPieSegment(centerX - radius, centerY - radius,
            diameter, diameter,
            static_cast<float>(startAngle) * float_Pi / 180.0f + i * anglePerSection,
            static_cast<float>(startAngle) * float_Pi / 180.0f + (i + 1) * anglePerSection,
            0.0);

        // Fülle den Bereich mit der aktuellen Farbe
        g.fillPath(sectionPath);
    }
    g.setColour(shape_colour);
    g.fillEllipse(centerX - radius * 0.75, centerY - radius * 0.75, radius * 1.5, radius * 1.5);
}





std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::readCSVFromString(const std::string& dataString)
{
    std::vector<float> alpha;
    std::vector<float> beta;
    std::vector<float> iterations;

    std::istringstream iss(dataString);
    std::string line;
    // Überspringe die erste Zeile (Header)
    std::getline(iss, line);
    while (std::getline(iss, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ';'); // Verwende ';' als Trennzeichen
        alpha.push_back(std::stof(cell));
        std::getline(ss, cell, ';');
        beta.push_back(std::stof(cell));
        std::getline(ss, cell, ';');
        iterations.push_back(std::stof(cell));
    }

    csvAlpha = alpha;
    csvBeta = beta;
    csvIterations = iterations;

    // Gib die Vektoren zurück
    return alpha, beta, iterations;
}


std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::getIterationsForAlpha(float targetAlpha) {
    std::vector<float> iterations;

    for (size_t i = 0; i < csvAlpha.size(); i++) {
        if (csvAlpha[i] == targetAlpha) {
            iterations.push_back(csvIterations[i]);
        }
    }

    // Fülle den Vektor mit Nullen auf, falls erforderlich
    //while (iterations.size() < 100) {
    //    iterations.push_back(0.0);
   //}

    // Begrenze den Vektor auf 100 Einträge, falls erforderlich
    //iterations.resize(100);

    return iterations;
}

Array<Colour> IPFSynthesizerVSTAudioProcessorEditor::generateColors(const std::vector<float>& iterations) {
    Array<Colour> colours;

    for (const auto& value : iterations) {
        // Überprüfe, ob der Wert über 300 oder unter 10 liegt
        if (value > 300 || value < 10) {
            colours.add(Colours::red); // Markiere den Wert als rot
        }
        else {
            colours.add(Colours::green); // Markiere den Wert als grün
        }
    }

    // Fülle den Rest des Arrays mit Standardfarbwerten (z. B. Weiß)
    //while (colours.size() < 100) {
    //    colours.add(Colours::white);
    //}

    return colours;
}



double IPFSynthesizerVSTAudioProcessorEditor::roundToTwoDecimalPlaces(double value) {
    double roundedValue = std::round(value * 100) / 100;
    return roundedValue;
}