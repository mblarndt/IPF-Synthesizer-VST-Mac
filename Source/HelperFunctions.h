/*
  ==============================================================================

    HelperFunctions.h
    Created: 22 Jul 2023 1:04:01pm
    Author:  Ben Arndt

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

using namespace std;

class HelperFunctions
{
public:
    float compressSample(float sample, float threshold = 0.95);
    float calculatePhaseshift(float delta);
};

