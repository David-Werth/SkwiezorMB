/*
  ==============================================================================

    LookAndFeel.h
    Created: 26 May 2024 9:08:22am
    Author:  David Werth

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "CustomButtons.h"


struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;
    
    void drawToggleButton (juce::Graphics &g,
                           juce::ToggleButton & toggleButton,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};

