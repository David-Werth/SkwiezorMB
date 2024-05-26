/*
  ==============================================================================

    Params.h
    Created: 26 May 2024 9:31:58am
    Author:  David Werth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Params
{
enum Names
{
    Low_mid_Crossover_Freq,
    Mid_high_Crossover_Freq,
    
    Threshold_Low_Band,
    Threshold_Mid_Band,
    Threshold_High_Band,
    
    Attack_Low_Band,
    Attack_Mid_Band,
    Attack_High_Band,
    
    Release_Low_Band,
    Release_Mid_Band,
    Release_High_Band,
    
    Ratio_Low_Band,
    Ratio_Mid_Band,
    Ratio_High_Band,
    
    Bypass_Low_Band,
    Bypass_Mid_Band,
    Bypass_High_Band,
    
    Mute_Low_Band,
    Mute_Mid_Band,
    Mute_High_Band,
    
    Solo_Low_Band,
    Solo_Mid_Band,
    Solo_High_Band,
    
    Gain_In,
    Gain_Out,
};

inline const std::map<Names, juce::String>& GetParams()
{
    static std::map<Names, juce::String> params =
    {
        {Low_mid_Crossover_Freq, "Low-Mid Crossover Freq"},
        {Mid_high_Crossover_Freq, "Mid-High Crossover Freq"},

        {Threshold_Low_Band, "Threshold Low Band"},
        {Threshold_Mid_Band, "Threshold Mid Band"},
        {Threshold_High_Band, "Threshold High Band"},

        {Attack_Low_Band, "Attack Low Band"},
        {Attack_Mid_Band, "Attack Mid Band"},
        {Attack_High_Band, "Attack High Band"},

        {Release_Low_Band, "Release Low Band"},
        {Release_Mid_Band, "Release Mid Band"},
        {Release_High_Band, "Release High Band"},

        {Ratio_Low_Band, "Ratio Low Band"},
        {Ratio_Mid_Band, "Ratio Mid Band"},
        {Ratio_High_Band, "Ratio High Band"},

        {Bypass_Low_Band, "Bypass Low Band"},
        {Bypass_Mid_Band, "Bypass Mid Band"},
        {Bypass_High_Band, "Bypass High Band"},
        
        {Mute_Low_Band, "Mute Low Band"},
        {Mute_Mid_Band, "Mute Mid Band"},
        {Mute_High_Band, "Mute High Band"},
        
        {Solo_Low_Band, "Solo Low Band"},
        {Solo_Mid_Band, "Solo Mid Band"},
        {Solo_High_Band, "Solo High Band"},
        
        {Gain_In, "Gain In"},
        {Gain_Out, "Gain Out"},
    };
    
    return params;
}
}
