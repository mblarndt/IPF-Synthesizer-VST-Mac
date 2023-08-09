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
    
    //Wavetable Selection Init
    String rb1 = String("sine");
    String rb2 = String("square");
    String rb3 = String("triangle");
    String rb4 = String("saw");
    radioButton_init(radioButton_13_sine, rb1, int(13));
    radioButton_13_sine.setToggleState(true, dontSendNotification);
    radioButton_init(radioButton_13_square, rb2, int(13));
    radioButton_init(radioButton_13_triangle, rb3, int(13));
    radioButton_init(radioButton_13_saw, rb4, int(13));


    //File Selection Setup
    String fileText = String("Select File");
    button_init(button_file, fileText);
    String helpText = String("!");
    button_init(button_help, helpText);
    button_help.setClickingTogglesState(true);
    
    //Gain Slider Setup
    dial_init(slider_gain, Slider::SliderStyle::LinearBarVertical, 0);
    resetRange(slider_gain, "gain");
    slider_gain.setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
    slider_gain.setNumDecimalPlacesToDisplay(0);

    //IPF-Parameter Init
    dial_init(dial_g, Slider::SliderStyle::Rotary, 1, 0, 4, 0.1);
    dial_init(dial_alpha, Slider::SliderStyle::Rotary, 0.5, 0, 1, 0.01);
    dial_alpha.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(1, 215, 50));
    dial_init(dial_beta, Slider::SliderStyle::Rotary, 0, 0, 1, 0.01);
    dial_init(dial_gamma, Slider::SliderStyle::Rotary, 0, 0, 1, 0.01);
    
    StringArray modeItems;
    modeItems.add("Fixed");
    modeItems.add("Threshold");
    //modeItems.add("Stable");
    //modeItems.add("Bif.");
    //modeItems.add("Chaos");
    dropdown_init(modeMenu, modeItems, 0);
    
    StringArray thresholdItems;
    thresholdItems.add("100%");
    thresholdItems.add("90%");
    thresholdItems.add("80%");
    thresholdItems.add("70%");
    thresholdItems.add("60%");
    dropdown_init(thresholdMenu, thresholdItems, 0);
    

    //Toggle Init
    toggle_init(toggle_bypass);
    toggle_bypass.setToggleState(true, NotificationType::sendNotification);
    toggle_init(toggle_ampmod);
    toggle_ampmod.setToggleState(true, NotificationType::sendNotification);
    toggle_init(toggle_phasemod);
    toggle_init(toggle_freqmod);
    toggle_init(toggle_fixstate);
    toggle_fixstate.setToggleState(true, NotificationType::sendNotification);

    //Mod-Influence
    dial_init(dial_phasemod, Slider::SliderStyle::Rotary, 1, 0, 2);
    dial_init(dial_ampmod, Slider::SliderStyle::Rotary, 1, 0, 10);
    dial_init(dial_freqmod, Slider::SliderStyle::Rotary, 1, 0, 50, 1);

    dial_ampmod.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGBA(0, 0, 0, 0));
    dial_phasemod.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGBA(0, 0, 0, 0));
    dial_freqmod.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGBA(0, 0, 0, 0));
    dial_ampmod.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 15);
    dial_phasemod.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 15);
    dial_freqmod.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 15);

    //Value Tree Setup
    gAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "g", dial_g);
    alphaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "alpha", dial_alpha);
    betaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "beta", dial_beta);
    gammaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gamma", dial_gamma);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", slider_gain);
    ampmodAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ampmod", dial_ampmod);
    phasemodAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "phasemod", dial_phasemod);
    freqmodAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "freqmod", dial_freqmod);
    ampmodToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "ampmodToggle", toggle_ampmod);
    freqmodToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "freqmodToggle", toggle_freqmod);
    phasemodToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "phasemodToggle", toggle_phasemod);

    addAndMakeVisible(plot);

    //Plot View Selection Init
    String rb5 = String("IPF");
    String rb6 = String("Signal");
    radioButton_init(radioButton_14_ipf, rb5, int(14));
    radioButton_init(radioButton_14_signal, rb6, int(14));
    radioButton_14_ipf.setToggleState(true, dontSendNotification);
    
    // Set Plot Parameters
    //plot.xLim(0, 500);
    plotSignal = false;
    yData[0] = calculateIPF(1, dial_alpha.getValue(), dial_beta.getValue(), dial_gamma.getValue(), plotSignal);
    plot.yLim(-2, 2);
    plot.xLim(0, 300);
    

    plot.plot(yData);


    
    setSize(800, 480);
    
    setLookAndFeel(&claf);

    //Load CSV Lists
    const char* txtData = reinterpret_cast<const char*>(BinaryData::results_fixed_csv);
    readCSVFromString(txtData, &fixedMode);
    
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_thresh_60_csv);
    readCSVFromString(txtData, &threshold60Mode);
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_thresh_70_csv);
    readCSVFromString(txtData, &threshold70Mode);
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_thresh_80_csv);
    readCSVFromString(txtData, &threshold80Mode);
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_thresh_90_csv);
    readCSVFromString(txtData, &threshold90Mode);
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_thresh_100_csv);
    readCSVFromString(txtData, &threshold100Mode);
    
    /*
    txtData = reinterpret_cast<const char*>(BinaryData::results_stable_csv);
    readCSVFromString(txtData, &stableMode);
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_bif_csv);
    readCSVFromString(txtData, &bifMode);
    
    txtData = reinterpret_cast<const char*>(BinaryData::results_chaos_csv);
    readCSVFromString(txtData, &chaosMode);
    */
    
    audioProcessor.chosenWavetable = "sine";
    
    helpBox = juce::ImageCache::getFromMemory(BinaryData::helppng_png, BinaryData::helppng_pngSize);
    
    randomCounter = -50;
    
    toggle_freqmod.setToggleState(false, true);
    audioProcessor.freqmod = false;
    audioProcessor.phasemod = false;
    audioProcessor.ampmod = true;
    
    dial_g.setEnabled(true);

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
    
    
    //Down Side - IPF Field
    paint_shape(g, Rectangle<int>(0, 334, 800, 145), shape_colour);
    
    //Right Side - Volume Field
    paint_shape(g, Rectangle<int>(695, 40, 105, 294), shape_colour);
    
    //Middle Field
    paint_shape(g, Rectangle<int>(0, 48, 508+187, 286), shape_colour);
    
    //Mod Field
    paint_shape(g, Rectangle<int>(0, 410, 186, 230), shape_colour);
    
    //Mod Field
    paint_shape(g, Rectangle<int>(0, 249, 186, 160), shape_colour);
    
    //Load Wave Field
    paint_shape(g, Rectangle<int>(0, 165, 186, 84), shape_colour);
    
    //Wavetable Field
    paint_shape(g, Rectangle<int>(0, 48, 186, 117), shape_colour);
    
    //Header Field
    paint_shape(g, Rectangle<int>(0, 0, 800, 48), shape_colour);
    

    String font = String("Arial");
    Colour text_colour = Colour::fromRGB(255, 255, 255);
    
    paint_label(g,dial_g, "Init. Value");
    //paint_label(g,dial_rate, "Rate");
    paint_label(g,dial_alpha, "Input Strength");
    paint_label(g,dial_beta, "1. Reflection");
    paint_label(g,dial_gamma, "2. Reflection");
    paint_text(g, font, 18, text_colour, String("Gain"), 745.5, 320);
    
    paint_text(g, font, 18, text_colour, String("Wavetable"), 93, 58 + 18);
    paint_text(g, font, 18, text_colour, String("Load Wave"), 93, 174 + 18);

    //Mod Window
    int spacing = 55;
    paint_text(g, font, 18, text_colour, String("Amp-Mod"), 15, 265 + 20, false);
    paint_text(g, font, 18, text_colour, String("Phase-Mod"), 15, 265 + spacing + 20, false);
    paint_text(g, font, 18, text_colour, String("Freq-Mod"), 15, 265+ spacing * 2 + 20, false);
    paint_text(g, font, 18, text_colour, String("Choose Initial"), 15, 265+ spacing * 2.8 + 25, false);
    paint_text(g, font, 18, text_colour, String("Value"), 15, 265+ spacing * 3.2 + 25, false);

    //Header
    paint_text(g, font, 18, text_colour, String("MBLA    |   IPF - Synthesizer "), 20.0, 15+ 18, false);
    paint_text(g, font, 18, text_colour, String("Help"), 700, 15+ 18, false);
    
    paint_text(g, font, 18, text_colour, String("Mode"), 710, 365, false);
    paint_text(g, font, 18, text_colour, String("Threshold"), 710, 430, false);
    


    // Größe und Position des Kreises
    int diameter = 67;
    // Zeichne den Kreis mit den Bereichen
    drawColorfulCircle(g, dial_alpha.getBounds().getX()+45, 406, diameter, alphaColours);
    drawColorfulCircle(g, dial_beta.getBounds().getX()+45, 406, diameter, betaColours);
    drawColorfulCircle(g, dial_gamma.getBounds().getX() + 45, 406, diameter, gammaColours);
    //updateCircleColors();
    
    if(button_help.getToggleState() == true) {
        g.drawImage(helpBox, Rectangle<float>(186, 48, 509, 291));
        plot.setVisible(false);
        radioButton_14_signal.setVisible(false);
        radioButton_14_ipf.setVisible(false);
    }
    else {
        plot.setVisible(true);
        radioButton_14_signal.setVisible(true);
        radioButton_14_ipf.setVisible(true);
    }
    
}

void IPFSynthesizerVSTAudioProcessorEditor::resized()
{
    
    //Bypass Toggle
    button_help.setBounds(750, 14, 23, 23);

    //Mod-Window
    int spacing = 55;
    int diameter = 23;
    toggle_ampmod.setBounds(150, 265, diameter, diameter);
    toggle_phasemod.setBounds(150, 265 + spacing, diameter, diameter);
    toggle_freqmod.setBounds(150, 265 + spacing * 2,diameter, diameter);
    toggle_fixstate.setBounds(150, 265 + spacing * 3.1,diameter, diameter);
    
    modeMenu.setBounds(710, 370, 80, 30);
    thresholdMenu.setBounds(710, 435, 80, 30);

    diameter = 50;
    dial_ampmod.setBounds(100, 252, diameter, diameter);
    dial_phasemod.setBounds(100, 252 + spacing, diameter, diameter);
    dial_freqmod.setBounds(100, 252 + spacing * 2, diameter, diameter);


    //Load Wavetable Window
    button_file.setBounds(36, 205, 114, 30);
    

    //Wavetable Selection Window
    radioButton_13_sine.setBounds(14.37, 87, 79.04, 27.54);
    radioButton_13_square.setBounds(92.21, 87, 79.04, 27.54);
    radioButton_13_triangle.setBounds(92.21, 114, 79.04, 27.54);
    radioButton_13_saw.setBounds(14.37, 114, 79.04, 27.54);


    //Gain Window
    slider_gain.setBounds(729, 60, 37.12, 220);
    
    // IPF-Parameter Window
    int startpos = 205;
    int y_pos = 369.5;
    int margin = 125;
    diameter = 90;
    //dial_rate.setBounds(0, 369.5, 90.0, 90.0);
    dial_g.setBounds(startpos, y_pos, diameter, diameter);
    dial_alpha.setBounds(startpos + margin, y_pos, diameter, diameter);
    dial_beta.setBounds(startpos + margin * 2, y_pos, diameter, diameter);
    dial_gamma.setBounds(startpos + margin * 3, y_pos, diameter, diameter);
    //slider_input.setBounds(215, 184, 250, 20);


    //IPF-Plot Window
    radioButton_14_ipf.setBounds(528, 70, 75, 25);
    radioButton_14_signal.setBounds(603, 70, 75, 25);
    plot.setBounds(185, 62, 501, 270);
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
void IPFSynthesizerVSTAudioProcessorEditor::dropdown_init(juce::ComboBox& comboBox, const StringArray& items, int defaultIndex)
{
    comboBox.addItemList(items, 1);
    comboBox.setSelectedId(defaultIndex + 1, juce::sendNotification);
    comboBox.onChange = [this, nameCopy = &comboBox]() { dropdownValueChanged(nameCopy); };
    addAndMakeVisible(comboBox);
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

        float new_beta_max = value;
        dial_beta.setRange(0.0, new_beta_max, 0.01);

        // Regel: alpha > beta
        if (audioProcessor.beta >= new_beta_max)
            audioProcessor.beta = new_beta_max;

        

        // dial_beta.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_beta_max, 0));

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

        //dial_gamma.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_gamma_max, 0));
        if(value <= dial_beta.getValue())
            dial_alpha.setValue(dial_beta.getValue());
        
        
        updateCircleColors(true, true, true);
        
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
            new_max = 0.01f;

        dial_gamma.setRange(0.0, new_max, 0.01);

        // Regel: gamma <= new_max
        if (dial_gamma.getValue() >= new_max)
            audioProcessor.gamma = new_max - 0.001;

        //dial_gamma.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_max, 0));
        updateCircleColors(true, true, true);
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
        reloadIPF();
    }
    else if (slider == &slider_gain) {
        float volume = std::pow(10.0f, slider->getValue() / 20.0f);
        audioProcessor.volume = volume;
    }
    else if (slider == &dial_ampmod) {
        audioProcessor.ampmod = slider->getValue();
    }
    else if (slider == &dial_phasemod) {
        audioProcessor.phasemod = slider->getValue();
    }
    else if (slider == &dial_freqmod) {
        audioProcessor.freqmod = slider->getValue();
    }
    //else if (slider == &dial_rate) {
    //    audioProcessor.ipf_rate = slider->getValue();
    //}
    
    if(toggle_fixstate.getToggleState() == false) {
        // Mindestwert und Maximalwert für die zufällige Zahl

        // Zufallszahlengenerator initialisieren
        if(modeMenu.getSelectedItemIndex() != 0)
            dial_g.setValue(1 / dial_alpha.getValue());
        //dial_g.setValue(dial_alpha.getValue() + (dial_alpha.getValue() * randomCounter * (1/(1/dial_alpha.getValue()))));
        //dial_g.setValue(dial_alpha.getValue() + (2 * (1/(100/audioProcessor.randomCounter))));
        //DBG(randomCounter);
    }
        //dial_g.setValue((1/dial_alpha.getValue()));


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
        reloadIPF();
    }
    
    repaint();

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
        else if (button == &toggle_phasemod) {
            bool value = button->getToggleState();
            if(value == true)
                toggle_freqmod.setToggleState(false, sendNotification);
            audioProcessor.phaseMod = value;
        }
        else if (button == &toggle_ampmod) {
            bool value = button->getToggleState();
            audioProcessor.ampMod = value;
        }
        else if (button == &toggle_freqmod) {
            bool value = button->getToggleState();
            if(value == true)
                toggle_phasemod.setToggleState(false, sendNotification);
            audioProcessor.freqMod = value;
        }
        else if (button == &toggle_fixstate) {
            bool value = button->getToggleState();
            //audioProcessor.freqMod = value;
            if(value == false) {
                dial_g.setEnabled(false);
            }
            else {
                dial_g.setEnabled(true);
            }
            audioProcessor.fixedG = value;
                
 
                
        }
        reloadIPF();
    }

    for (auto* name : textButtons)
    {
        // Handle slider changes here
        if (button == name)
        {
            String btn = button->getButtonText();
            //DBG(btn + ": Button Click");
                
        }
        
    }

    for (auto* name : radioButtons)
    {
        // Handle slider changes here
        if (button == name)
        {
            if (button->getRadioGroupId() == 13) {
                String btn = button->getButtonText();
                //DBG(btn + ": Button Click");
                audioProcessor.chosenWavetable = btn;
                updateWaveTablePath();

            }
            if (button->getRadioGroupId() == 14) {
                String btn = button->getButtonText();
                
                if (btn != previousBtn) {
                    //DBG(btn + ": Button Click");

                    if (btn == "Signal") {
                        plotSignal = true;
                        plot.xLim(0, 6414);
                        plot.yLim(-2, 2);
                        plot.setBounds(185, 62, 510, 270);
                    }
                    else {
                        plotSignal = false;
                        plot.xLim(0, 300);
                        plot.yLim(-2, 2);
                        plot.setBounds(185, 62, 510, 270);
                    }
                    reloadIPF();
                }

                previousBtn = btn;

            }
            reloadIPF();
        }
    }
}

void IPFSynthesizerVSTAudioProcessorEditor::dropdownValueChanged(juce::ComboBox* comboBox) {
    String modeValue;
    String thresholdValue;
    
    if (comboBox == &modeMenu) {
        modeValue = comboBox->getText();
        
        if (modeValue == "Fixed") {
            dial_g.setValue(1);
            //dial_g.setEnabled(false);
        }
        else {
            dial_g.setRange(0,4,0.1);
            //dial_g.setEnabled(true);
        }
        
        if(modeValue != "Threshold") {
            thresholdMenu.setEnabled(false);
            dial_g.setEnabled(true);
        }
        else {
            thresholdMenu.setEnabled(true);
            dial_g.setEnabled(false);
            toggle_fixstate.setToggleState(false, true);
        }
        
        if(modeValue == "Fixed")
            toggle_fixstate.setToggleState(true, sendNotification);
    }
    else if(comboBox == &thresholdMenu) {
        thresholdValue = comboBox->getText();
    }
    updateShownList(modeValue, thresholdValue);
    updateCircleColors();
    repaint();
    
}

void IPFSynthesizerVSTAudioProcessorEditor::updateShownList(String mode, String threshold) {
    
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
    float g0 = gVal;
    float g = g0;
    float g_pre_1 = 1;
    float g_pre_2 = 1;
    float g_plus = 0;

    
    float gs = alphaVal + betaVal + gammaVal;
    
    float g_ampmod;
    float g_signalmod;
    


    std::vector<float> g_array;
    std::vector<float> g_plus_normed_array;
    std::vector<float> g_ampmod_array;
    std::vector<float> signal_ampmod_array;
    std::vector<float> g_interp;
    std::vector<float> modified_interp; // Neues Array für den modifizierten Output
    std::vector<float> phaseshift_array;
    std::vector<float> freqshift_array;
    std::vector<float> output;
    std::vector<float> nullVector(500, 0);

    
    int max_iterations = 300;  // Maximale Anzahl von Iterationen
    if(alphaVal != 0 and g != 0) {
        for (int iteration = 0; iteration < max_iterations; ++iteration) {
            
            g_plus = g - std::log((1 / alphaVal) * (g - (betaVal * std::exp(g - g_pre_1) + gammaVal * std::exp(g - g_pre_2))));
            
            //Prüfen ob Zahl komplex ist
            if (!std::isfinite(g_plus)) {
                return nullVector;
            }
        
            
            // Neuen Wert dem Array hinzufügen
            g_array.push_back(g_plus);
            
            
            // Normiere Wert und füge sie einem Array hinzu
            float g_plus_norm = ((g_plus) / gs) - 1;
            g_plus_normed_array.push_back(g_plus_norm);

            
            // Berechne Werte Differenz
            float g_delta = abs(g - g_plus);
            
            //Berechne Frequenzmodulation
            float freqshift = g_delta;
            freqshift_array.push_back(freqshift);

            
            float phaseshift = helper.calculatePhaseshift(g_delta);
            phaseshift_array.push_back(phaseshift);
            
            //normierten Zustand mit Faktor vom Regler verrechnen
            g_ampmod = g_plus_norm * dial_ampmod.getValue();
            //Wieder zu ursprünglicher Form bringen
            g_signalmod = (g_ampmod + 1) * gs;
            g_ampmod_array.push_back(g_signalmod);
            
            //Werte nach hinten rücken
            g_pre_2 = g_pre_1;
            g_pre_1 = g;
            g = g_plus;
        }
        
        //output = g_ampmod_array;
        
        if (calcSignal == true) {
            float sample;
            
            std::vector<float> wt = wtg.generateWaveTable(audioProcessor.chosenWavetable);
            std::vector<float> wt_phasemod;
            std::vector<float> wt_freqemod;
            std::vector<float> active_wt = wt;
            std::vector<float> used_vector;
            

            for (int i = 0; i < g_ampmod_array.size(); ++i) {
                float value = g_ampmod_array[i];
                
                
                if (toggle_phasemod.getToggleState() == true) {
                    wt_phasemod = wtg.generateSinusWave(1, phaseshift_array[i] * 10 *  dial_phasemod.getValue());
                }
                
                if(toggle_freqmod.getToggleState()== true) {
                    wt_freqemod = interpolateArray(wt, wt.size() + (1 + phaseshift_array[i] * dial_freqmod.getValue()));
                    active_wt = wt_freqemod;
                }
                
                if(toggle_ampmod.getToggleState() == true) {
                    //Alle Werte der Phase mit dem Wert verrechnen
                    for (int p = 0; p < active_wt.size(); p++) {
                        
                        if (toggle_phasemod.getToggleState() == true) {
                            sample = abs(value) * (active_wt[p] / 2) + (wt_phasemod[p] / 2);
                        }
                        else {
                            sample = abs(value) * active_wt[p];
                        }
                        //Compress if over Theshold of over 0.95
                        float compressed_sample = helper.compressSample(sample);
                        signal_ampmod_array.push_back(compressed_sample);
                    }
                }
                else {
                    //Alle Samples entsprechen dem wavetable
                    for (int p = 0; p < active_wt.size(); p++) {
                        if (toggle_phasemod.getToggleState() == true) {
                            sample = (wt[p] / 2) + (wt_phasemod[p] / 2);
                        }
                        else {
                            sample = active_wt[p];
                        }
                        //Compress if over Theshold of over 0.95
                        float compressed_sample = helper.compressSample(sample);
                        signal_ampmod_array.push_back(compressed_sample);
                    }
                }
                
            }
            output = signal_ampmod_array;
        }
    
        else {
            for (int i = max_iterations; i < 6400; ++i) {
                g_ampmod_array.push_back(0);
            }
            output = g_ampmod_array;
        }
    }
    else {
        return nullVector;
    }
    return output;
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



std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::readCSVFromString(const std::string& dataString , csvVectors* list)
{
    std::vector<float> g0;
    std::vector<float> alpha;
    std::vector<float> beta;
    std::vector<float> gamma;
    std::vector<float> behaviour;
    std::vector<float> percent;
    
    std::istringstream iss(dataString);
    std::string line;
    // Überspringe die erste Zeile (Header)
    std::getline(iss, line);
    
    while (std::getline(iss, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::getline(ss, cell, ';'); // Verwende ';' als Trennzeichen
        g0.push_back(std::stof(cell));
        std::getline(ss, cell, ';'); // Verwende ';' als Trennzeichen
        alpha.push_back(std::stof(cell));
        std::getline(ss, cell, ';');
        beta.push_back(std::stof(cell));
        std::getline(ss, cell, ';');
        gamma.push_back(std::stof(cell));
        std::getline(ss, cell, ';');
        percent.push_back(std::stof(cell));
        std::getline(ss, cell, ';');
        behaviour.push_back(std::stof(cell));
    }
    
    
    list->g0 = g0;
    list->alpha = alpha;
    list->beta = beta;
    list->gamma = gamma;
    list->behaviour = behaviour;
    list->percent = percent;

    // Gib die Vektoren zurück
    return alpha;
}


std::pair<std::vector<float>, std::vector<float>> IPFSynthesizerVSTAudioProcessorEditor::getAlphaPercentage(float targetg0,float targetAlpha, float targetBeta, float targetGamma, csvVectors usedVector)
{
    std::vector<float> percentage;
    std::vector<float> behaviour;
    
    float g0 = roundToTwoDecimalPlaces(targetg0);
    float alpha = roundToTwoDecimalPlaces(targetAlpha);
    float beta = roundToTwoDecimalPlaces(targetBeta);
    float gamma = roundToTwoDecimalPlaces(targetGamma);
    
    for (size_t i = 0; i < usedVector.g0.size(); i++) {
        if (usedVector.g0[i] == g0 && usedVector.beta[i] == beta && usedVector.gamma[i] == gamma) {
            percentage.push_back(usedVector.percent[i]);
            behaviour.push_back(usedVector.behaviour[i]);
        }
    }

    return make_pair(behaviour, percentage);
}

std::pair<std::vector<float>, std::vector<float>> IPFSynthesizerVSTAudioProcessorEditor::getBetaPercentage(float targetg0,float targetAlpha, float targetBeta, float targetGamma, csvVectors usedVector) {
    
    std::vector<float> percentage;
    std::vector<float> behaviour;
    float g0 = roundToTwoDecimalPlaces(targetg0);
    float alpha = roundToTwoDecimalPlaces(targetAlpha);
    float beta = roundToTwoDecimalPlaces(targetBeta);
    float gamma = roundToTwoDecimalPlaces(targetGamma);

    for (size_t i = 0; i < usedVector.g0.size(); i++) {
        if (usedVector.g0[i] == g0 && usedVector.alpha[i] == alpha && usedVector.gamma[i] == gamma) {
            percentage.push_back(usedVector.percent[i]);
            behaviour.push_back(usedVector.behaviour[i]);
        }
    }

    return std::make_pair(behaviour, percentage);
}

std::pair<std::vector<float>, std::vector<float>> IPFSynthesizerVSTAudioProcessorEditor::getGammaPercentage(float targetg0,float targetAlpha, float targetBeta, float targetGamma, csvVectors usedVector)
{
    std::vector<float> percentage;
    std::vector<float> behaviour;
    
    float g0 = roundToTwoDecimalPlaces(targetg0);
    float alpha = roundToTwoDecimalPlaces(targetAlpha);
    float beta = roundToTwoDecimalPlaces(targetBeta);
    float gamma = roundToTwoDecimalPlaces(targetGamma);

    for (size_t i = 0; i < usedVector.g0.size(); i++) {
        if (usedVector.g0[i] == g0 && usedVector.alpha[i] == alpha && usedVector.beta[i] == beta) {
            percentage.push_back(usedVector.percent[i]);
            behaviour.push_back(usedVector.behaviour[i]);
        }
    }

    return std::make_pair(behaviour, percentage);
}

Array<Colour> IPFSynthesizerVSTAudioProcessorEditor::generateColors(const std::vector<float>& behaviour, const std::vector<float>& percentage) {
    Array<Colour> colours;

    
    int mode = modeMenu.getSelectedItemIndex();
    //int threshold = thresholdMenu.getSelectedItemIndex();
    

        for (const auto& value : behaviour) {
            if (value == 0 or value == 5)
                colours.add(Colours::orangered); // Markiere den Wert als rot
            else if (value == 1)
                colours.add(Colours::forestgreen); // Markiere den Wert als grün
            else if (value == 2)
                colours.add(Colours::blueviolet); // Markiere den Wert als grün
            else if (value == 3)
                colours.add(Colours::black); // Markiere den Wert als grün
            else if (value == 6)
                colours.add(Colours::white); // Markiere den Wert als grün
            }
    
    
    /*
    else if (mode == 2 or mode == 3 or mode == 4) {
        for (const auto& value : percentage) {
            // Überprüfe, ob der Wert über 300 oder unter 10 liegt
                colours.add(Colour::fromRGBA(25, 190, 0, (value / 100) * 255)); // Markiere den Wert als rot
        }
    }
    */

    return colours;
}

void IPFSynthesizerVSTAudioProcessorEditor::updateCircleColors(bool updateAlpha, bool updateBeta, bool updateGamma) {
    float g0val = dial_g.getValue();
    float alphaval = dial_alpha.getValue();
    float betaval = dial_beta.getValue();
    float gammaval = dial_gamma.getValue();
    
    std::vector<float> behaviour;
    std::vector<float> percentage;
    
    int mode = modeMenu.getSelectedItemIndex();
    int threshold = thresholdMenu.getSelectedItemIndex();
    csvVectors selectedVector;
    
    if(mode == 0) {selectedVector = fixedMode;}
    else if(mode == 1) {
        if(threshold == 0) {selectedVector = threshold100Mode;}
        else if(threshold == 1) {selectedVector = threshold90Mode;}
        else if(threshold == 2) {selectedVector = threshold80Mode;}
        else if(threshold == 3) {selectedVector = threshold70Mode;}
        else if(threshold == 4) {selectedVector = threshold60Mode;}
    }
    //else if(mode == 2) {selectedVector = stableMode;}
    //else if(mode == 3) {selectedVector = bifMode;}
    //else if(mode == 4) {selectedVector = chaosMode;}
    
    //DBG(threshold);
    
    if(mode != 0) {g0val = 0;}
    
    auto result = getAlphaPercentage(g0val,alphaval, betaval, gammaval, selectedVector);
    
    if(updateAlpha == true ) {
        auto result = getAlphaPercentage(g0val,alphaval, betaval, gammaval, selectedVector);
        behaviour = result.first;
        percentage = result.second;
        alphaColours = generateColors(behaviour, percentage);
    }
    
    if(updateBeta == true) {
        result = getBetaPercentage(g0val, alphaval,betaval, gammaval, selectedVector);
        behaviour = result.first;
        percentage = result.second;
        betaColours = generateColors(behaviour, percentage);
    }
    
    if(updateBeta == true) {
        result = getGammaPercentage(g0val, alphaval, betaval,gammaval, selectedVector);
        behaviour = result.first;
        percentage = result.second;
        gammaColours = generateColors(behaviour, percentage);
    }
}



float IPFSynthesizerVSTAudioProcessorEditor::roundToTwoDecimalPlaces(float value) {
    float roundedValue = std::round(value * 100) / 100;
    return roundedValue;
}

void IPFSynthesizerVSTAudioProcessorEditor::reloadIPF() {
    yData[0] = calculateIPF(dial_g.getValue(), dial_alpha.getValue(), dial_beta.getValue(), dial_gamma.getValue(), plotSignal);
    plot.plot(yData); // Plot using time values on x-axis
}

// Lineare Interpolationsfunktion
float IPFSynthesizerVSTAudioProcessorEditor::linearInterpolation(float x, float x0, float y0, float x1, float y1) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

// Funktion zum Stauchen oder Strecken des Arrays mit Interpolation
std::vector<float> IPFSynthesizerVSTAudioProcessorEditor::interpolateArray(const std::vector<float>& inputArray, int newLength) {
    std::vector<float> resultArray(newLength);

    float scaleX = static_cast<float>(inputArray.size() - 1) / (newLength - 1);

    for (int i = 0; i < newLength; ++i) {
        float x = i * scaleX;
        int x0 = static_cast<int>(x);
        int x1 = x0 + 1;

        if (x1 >= inputArray.size()) {
            resultArray[i] = inputArray.back(); // Der letzte Wert wird für Werte außerhalb des ursprünglichen Arrays verwendet
        } else {
            resultArray[i] = linearInterpolation(x, x0, inputArray[x0], x1, inputArray[x1]);
        }
    }

    return resultArray;
}
