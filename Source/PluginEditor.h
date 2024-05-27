/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/LookAndFeel.h"
#include "GUI/UtilityComponents.h"
#include "GUI/GlobalControls.h"
#include "GUI/CompressorBandControls.h"

#include "GUI/SpectrumAnalyzer.h"

class SkwiezorMBAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SkwiezorMBAudioProcessorEditor (SkwiezorMBAudioProcessor&);
    ~SkwiezorMBAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LookAndFeel lnf;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SkwiezorMBAudioProcessor& audioProcessor;
    
    Placeholder controlBar /*analyzer, */ /*globalControls,*/ /*bandControls*/;
    GlobalControls globalControls { audioProcessor.apvts };
    CompressorBandControls bandControls { audioProcessor.apvts };
    SpectrumAnalyzer analyzer { audioProcessor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SkwiezorMBAudioProcessorEditor)
};
