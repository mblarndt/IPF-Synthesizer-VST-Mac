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

    dial_init(slider_volume, Slider::SliderStyle::LinearBar, 0);
    slider_volume.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    slider_volume.setRange(-100.0f, 20.0f);
    slider_volume.setTextBoxStyle(Slider::TextBoxLeft, false, 80, 20);
    slider_volume.setNumDecimalPlacesToDisplay(0);

    dial_init(dial_g, Slider::SliderStyle::Rotary, 1);
    dial_init(dial_alpha, Slider::SliderStyle::Rotary, 0.5);
    dial_alpha.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(1, 215, 30));

    dial_init(dial_beta, Slider::SliderStyle::Rotary, 0.3);
    dial_init(dial_gamma, Slider::SliderStyle::Rotary, 0.2);
    dial_init(dial_rate, Slider::SliderStyle::Rotary, 1);
    dial_rate.setRange(0.25, 4.0, 0.25);


    setSize(800, 479);

    audioProcessor.alpha = 0.5;
    audioProcessor.beta = 0.47;
    audioProcessor.gamma = 0.455;
    audioProcessor.volume = 1;
    audioProcessor.ipf_rate = 1;
    

    setLookAndFeel(&claf);
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
    
    Colour shape_colour = Colour::fromRGB(58, 58, 58);
    
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
    
    paint_text(g, font, 18, text_colour, String("Rate"), dial_rate.getX() + (dial_rate.getWidth() / 2), dial_rate.getY());
    paint_text(g, font, 18, text_colour, String("g"), dial_g.getX() + (dial_g.getWidth() / 2), dial_g.getY());
    paint_text(g, font, 18, text_colour, String("Input Strength"), dial_alpha.getX() + (dial_alpha.getWidth() / 2), dial_alpha.getY());
    paint_text(g, font, 18, text_colour, String("1. Reflection"), dial_beta.getX() + (dial_beta.getWidth() / 2), dial_beta.getY());
    paint_text(g, font, 18, text_colour, String("2. Reflection"), dial_gamma.getX() + (dial_gamma.getWidth() / 2), dial_gamma.getY());
    paint_text(g, font, 18, text_colour, String("Volume"), 745.5, 440);
    
    paint_text(g, font, 18, text_colour, String("Wavetable"), 93, 58 + 18);
    paint_text(g, font, 18, text_colour, String("Load Wave"), 93, 174 + 18);
    paint_text(g, font, 18, text_colour, String("G-Delta"), 30, 267 + 18, false);
    paint_text(g, font, 18, text_colour, String("f-Mod"), 30, 302 + 18, false);
    paint_text(g, font, 18, text_colour, String("MBLA    |   IPF - Synthesizer"), 40.0, 15+ 18, false);
    
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
    
    slider_volume.setBounds(729, 92.21, 37.12, 306.56);
    
    int startpos = 25;
    dial_rate.setBounds(startpos, 369.5, 90.0, 90.0);
    dial_g.setBounds(startpos + margin, 369.5, 90.0, 90.0);
    dial_alpha.setBounds(startpos + margin * 2, 369.5, 90.0, 90.0);
    dial_beta.setBounds(startpos + margin * 3, 369.5, 90.0, 90.0);
    dial_gamma.setBounds(startpos + margin * 4, 369.5, 90.0, 90.0);

}
void IPFSynthesizerVSTAudioProcessorEditor::dial_init(juce::Slider& name, Slider::SliderStyle style, float initValue) {
    sliders.add(&name);
    name.setSliderStyle(style);
    name.setRange(0.0, 1.0, 0.001);
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

void IPFSynthesizerVSTAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    for (auto* name : sliders)
    {
        // Handle slider changes here
        if (slider == name)
        {
            float value = slider->getValue();
            DBG(" changed to " + String(value));
        }
    }
    if (slider == &dial_alpha) {
        float value = slider->getValue();
        if(value == 0)
            value = 0.00001;
        audioProcessor.alpha = value;
        

        float new_beta_max = value;
        dial_beta.setRange(0.0, new_beta_max, 0.001);
        if (audioProcessor.beta >= new_beta_max)
            audioProcessor.beta = new_beta_max;

        dial_beta.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_beta_max, 0));


        float new_gamma_max = dial_beta.getValue();
        if (new_gamma_max >= (value - dial_beta.getValue())) {
            new_gamma_max = dial_alpha.getValue() - dial_beta.getValue();
        }

        dial_gamma.setRange(0.0, new_gamma_max, 0.001);

        if (dial_gamma.getValue() >= new_gamma_max)
            audioProcessor.gamma = new_gamma_max;

        dial_gamma.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_gamma_max, 0));
        dial_beta.repaint();
        dial_gamma.repaint();

    }
    else if (slider == &dial_beta) {
        float value = slider->getValue();
        if(value == 0)
            value = 0.00001;
        audioProcessor.beta = value;

        float new_max = value;

        if (new_max > (dial_alpha.getValue() - value)) {
            new_max = dial_alpha.getValue() - value;
        }

        dial_gamma.setRange(0.0, new_max, 0.001);

        if (dial_gamma.getValue() >= new_max)
            audioProcessor.gamma = new_max;

        dial_gamma.setColour(Slider::ColourIds::thumbColourId, Colour::fromRGB(250, 250 * new_max, 0));
        dial_gamma.repaint();
    }
    else if (slider == &dial_gamma) {
        float value = slider->getValue();
        if(value == 0)
            value = 0.00001;
        audioProcessor.gamma = value;
        
    }
    else if (slider == &dial_g) {
        audioProcessor.g = slider->getValue();
    }
    else if (slider == &slider_volume) {
        float volume = std::pow(10.0f, slider->getValue() / 20.0f);
        audioProcessor.volume = volume;
    }
    else if (slider == &dial_rate) {
        audioProcessor.ipf_rate = slider->getValue();
    }

}

void IPFSynthesizerVSTAudioProcessorEditor::buttonValueChanged(juce::Button* button)
{
    for (auto* name : toggleButtons)
    {
        // Handle slider changes here
        if (button == name)
        {
            String btn = button->getButtonText();
            float value = button->getToggleState();
            DBG(btn + " changed to " + String(value));
        }
        if (button == &button_file) {
            openFileAsync();
            for (auto* b : radioButtons) {
                b->setToggleState(true, NotificationType::dontSendNotification);
            }
                
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

            String btn = button->getButtonText();
            DBG(btn + ": Button Click");
            audioProcessor.chosenWavetable = btn;
            updateWaveTablePath();
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
