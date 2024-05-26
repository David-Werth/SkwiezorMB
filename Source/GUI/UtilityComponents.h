/*
  ==============================================================================

    UtilityComponents.h
    Created: 26 May 2024 9:18:26am
    Author:  David Werth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct Placeholder : juce::Component
{
    Placeholder();
    
    void paint(juce::Graphics& g) override;
    
    juce::Colour customColor;
};

struct RotarySlider : juce::Slider
{
    RotarySlider();
};
