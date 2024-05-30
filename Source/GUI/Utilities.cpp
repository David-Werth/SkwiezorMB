/*
  ==============================================================================

    Utilities.cpp
    Created: 26 May 2024 9:23:58am
    Author:  David Werth

  ==============================================================================
*/

#include "Utilities.h"

juce::String getValString(const juce::RangedAudioParameter& param, bool getLow, juce::String suffix)
{
    juce::String str;
    
    auto val = getLow ? param.getNormalisableRange().start : param.getNormalisableRange().end;
    
    bool useK = truncateKiloValue(val);
    str << val;
    
    if ( useK )
        str << "k";
    
    str << suffix;
    
    return str;
};

juce::Rectangle<int> drawModuleBackground(juce::Graphics &g, juce::Rectangle<int> bounds)
{
    using namespace juce;
    g.setColour(Colours::darkslategrey);
    g.fillAll();
    
    auto localBounds = bounds;
    
    bounds.reduce(3, 3);
    g.setColour(Colours::black);
    g.fillRoundedRectangle(bounds.toFloat(), 3);
    
    g.drawRect(localBounds);
    
    return bounds;
}
