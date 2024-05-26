/*
  ==============================================================================

    CustomButtons.h
    Created: 26 May 2024 9:15:38am
    Author:  David Werth

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct PowerButton : juce::ToggleButton { };

struct AnalyzerButton : juce::ToggleButton
{
    void resized() override;
    
    juce::Path randomPath;
};
