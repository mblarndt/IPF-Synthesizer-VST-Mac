/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 24 Apr 2023 11:08:44pm
    Author:  Ben

  ==============================================================================
*/
#include "JuceHeader.h"

#pragma once
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override
    {
        auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
        auto fill = juce::Colour::fromRGB(175, 61, 255);

        auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = 4.0f;
        auto arcRadius = radius - lineW * 0.5f;

        juce::Path backgroundArc;
        backgroundArc.addCentredArc (bounds.getCentreX(),
                                     bounds.getCentreY(),
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     rotaryStartAngle,
                                     rotaryEndAngle,
                                     true);

        g.setColour (outline);
        g.strokePath (backgroundArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc (bounds.getCentreX(),
                                    bounds.getCentreY(),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    rotaryStartAngle,
                                    toAngle,
                                    true);

            g.setColour (fill);
            g.strokePath (valueArc, juce::PathStrokeType (lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        auto thumbWidth = lineW * 2.0f;
        juce::Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - juce::MathConstants<float>::halfPi),
                                       bounds.getCentreY() + arcRadius * std::sin (toAngle - juce::MathConstants<float>::halfPi));

        g.setColour (slider.findColour (juce::Slider::thumbColourId));
        g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
    }
    
    

    
    void drawTickBox(juce::Graphics& g, juce::Component& /*component*/,
        float x, float y, float w, float h,
        const bool ticked,
        const bool isEnabled,
        const bool /*shouldDrawButtonAsHighlighted*/,
        const bool shouldDrawButtonAsDown) override
    {
        onImage = juce::ImageCache::getFromMemory(BinaryData::on_png, BinaryData::on_pngSize);
        offImage = juce::ImageCache::getFromMemory(BinaryData::off_png, BinaryData::off_pngSize);

        if (ticked)
        {
            g.drawImageWithin(onImage, 0, 0, 23, 23, juce::RectanglePlacement::stretchToFit);
        }
        else
            g.drawImageWithin(offImage, 0, 0, 23, 23, juce::RectanglePlacement::stretchToFit);
    }
    



private:
    juce::Image onImage;
    juce::Image offImage;
};
