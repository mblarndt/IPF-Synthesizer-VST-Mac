/*
  ==============================================================================

    helper-functions.cpp
    Created: 22 Jul 2023 1:04:01pm
    Author:  Ben Arndt

  ==============================================================================
*/

#include "HelperFunctions.h"


float HelperFunctions::compressSample(float sample, float threshold)
{
    // Falls der Sample-Wert den Threshold überschreitet, komprimiere ihn
    if (std::abs(sample) >= threshold) {
        // Berechne den Kompressionsfaktor
        float compressionFactor = threshold / std::abs(sample);

        // Komprimiere den Sample-Wert
        sample *= compressionFactor;
    }

    return sample;
}

float HelperFunctions::calculatePhaseshift(float delta) {
    const float maxPhaseshift = 1.0;
    const float minPhaseshift = -1.0;
    float phaseshift = fmod(delta, 2.0); // Sicherstellen, dass der Wert zwischen 0 und 1 liegt

    // Skaliere den Phasenverschiebungswert auf den Bereich zwischen -1 und 1
    if (phaseshift > maxPhaseshift) {
        phaseshift -= 2.0;
    } else if (phaseshift < minPhaseshift) {
        phaseshift += 2.0;
    }
    return phaseshift;
}
