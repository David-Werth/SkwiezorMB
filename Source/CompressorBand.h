/*
  ==============================================================================

    CompressorBand.h
    Created: 26 May 2024 9:34:44am
    Author:  David Werth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

struct CompressorBand
{
    juce::AudioParameterFloat* attack { nullptr };
    juce::AudioParameterFloat* release { nullptr };
    juce::AudioParameterFloat* threshold { nullptr };
    juce::AudioParameterChoice* ratio { nullptr };
    juce::AudioParameterBool* bypass { nullptr };
    juce::AudioParameterBool* mute { nullptr };
    juce::AudioParameterBool* solo { nullptr };

    
    void prepare(const juce::dsp::ProcessSpec& spec);
    
    void updateCompressorSettings();
    
    void process(juce::AudioBuffer<float>& buffer);
    
private:
    juce::dsp::Compressor<float> compressor;
};
