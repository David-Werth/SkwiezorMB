/*
  ==============================================================================

    PathProducer.h
    Created: 27 May 2024 4:04:53pm
    Author:  David Werth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "FFTDataGenerator.h"
#include "AnalyzerPathGenerator.h"
#include "../PluginProcessor.h"

struct PathProducer
{
    PathProducer(SingleChannelSampleFifo<SkwiezorMBAudioProcessor::BlockType>& scsf) :
    leftChannelFifo(&scsf)
    {
        leftChannelFFTDataGenerator.changeOrder(FFTOrder::order2048);
        monoBuffer.setSize(1, leftChannelFFTDataGenerator.getFFTSize());
    }
    void process(juce::Rectangle<float> fftBounds, double sampleRate);
    juce::Path getPath() { return leftChannelFFTPath; }
    
    void updatenegativeInfinity(float nf) { negativeInfinity = nf; }
private:
    SingleChannelSampleFifo<SkwiezorMBAudioProcessor::BlockType>* leftChannelFifo;
    
    juce::AudioBuffer<float> monoBuffer;
    
    FFTDataGenerator<std::vector<float>> leftChannelFFTDataGenerator;
    
    AnalyzerPathGenerator<juce::Path> pathProducer;
    
    juce::Path leftChannelFFTPath;
    
    float negativeInfinity { -48.f };
};
