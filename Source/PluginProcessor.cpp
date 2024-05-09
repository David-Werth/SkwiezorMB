/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SkwiezorMBAudioProcessor::SkwiezorMBAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    using namespace Params;
    const auto& params = GetParams();
    
    auto floatHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    
    auto choiceHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    
    auto boolHelper = [&apvts = this->apvts, &params](auto& param, const auto& paramName)
    {
        param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(paramName)));
        jassert(param != nullptr);
    };
    
    floatHelper(lowBandComp.attack,     Names::Attack_Low_Band);
    floatHelper(lowBandComp.release,    Names::Release_Low_Band);
    floatHelper(lowBandComp.threshold,  Names::Threshold_Low_Band);
    
    floatHelper(midBandComp.attack,     Names::Attack_Mid_Band);
    floatHelper(midBandComp.release,    Names::Release_Mid_Band);
    floatHelper(midBandComp.threshold,  Names::Threshold_Mid_Band);
    
    floatHelper(highBandComp.attack,    Names::Attack_High_Band);
    floatHelper(highBandComp.release,   Names::Release_High_Band);
    floatHelper(highBandComp.threshold, Names::Threshold_High_Band);
    
    choiceHelper(lowBandComp.ratio,     Names::Ratio_Low_Band);
    choiceHelper(midBandComp.ratio,     Names::Ratio_Mid_Band);
    choiceHelper(highBandComp.ratio,    Names::Ratio_High_Band);
    
    boolHelper(lowBandComp.bypass,      Names::Bypass_Low_Band);
    boolHelper(midBandComp.bypass,      Names::Bypass_Mid_Band);
    boolHelper(highBandComp.bypass,     Names::Bypass_High_Band);
    
    boolHelper(lowBandComp.mute,        Names::Mute_Low_Band);
    boolHelper(midBandComp.mute,        Names::Mute_Mid_Band);
    boolHelper(highBandComp.mute,       Names::Mute_High_Band);
    
    boolHelper(lowBandComp.solo,        Names::Solo_Low_Band);
    boolHelper(midBandComp.solo,        Names::Solo_Mid_Band);
    boolHelper(highBandComp.solo,       Names::Solo_High_Band);
    
    floatHelper(lowMidCrossover,        Names::Low_mid_Crossover_Freq);
    floatHelper(midHighCrossover,       Names::Mid_high_Crossover_Freq);
    
    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    
    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
}

SkwiezorMBAudioProcessor::~SkwiezorMBAudioProcessor()
{
}

//==============================================================================
const juce::String SkwiezorMBAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SkwiezorMBAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SkwiezorMBAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SkwiezorMBAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SkwiezorMBAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SkwiezorMBAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SkwiezorMBAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SkwiezorMBAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SkwiezorMBAudioProcessor::getProgramName (int index)
{
    return {};
}

void SkwiezorMBAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SkwiezorMBAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    for ( auto& comp : compressors )
        comp.prepare(spec);
    
    LP1.prepare(spec);
    HP1.prepare(spec);
    
    AP2.prepare(spec);
    
    LP2.prepare(spec);
    HP2.prepare(spec);

    for ( auto& buffer : filterBuffers )
    {
        buffer.setSize(spec.numChannels, samplesPerBlock);
    }
}

void SkwiezorMBAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SkwiezorMBAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SkwiezorMBAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for ( auto& compressor : compressors )
        compressor.updateCompressorSettings();
    
    for ( auto& filterBuffer : filterBuffers )
        filterBuffer = buffer;
    
    auto lowMidCutoffFreq = lowMidCrossover->get();
    LP1.setCutoffFrequency(lowMidCutoffFreq);
    HP1.setCutoffFrequency(lowMidCutoffFreq);
    
    auto midHighCutoffFreq = midHighCrossover->get();
    AP2.setCutoffFrequency(midHighCutoffFreq);
    LP2.setCutoffFrequency(midHighCutoffFreq);
    HP2.setCutoffFrequency(midHighCutoffFreq);
    
    auto filterBuffer0Block = juce::dsp::AudioBlock<float>(filterBuffers[0]);
    auto filterBuffer1Block = juce::dsp::AudioBlock<float>(filterBuffers[1]);
    auto filterBuffer2Block = juce::dsp::AudioBlock<float>(filterBuffers[2]);
    
    auto filterBuffer0Context = juce::dsp::ProcessContextReplacing<float>(filterBuffer0Block);
    auto filterBuffer1Context = juce::dsp::ProcessContextReplacing<float>(filterBuffer1Block);
    auto filterBuffer2Context = juce::dsp::ProcessContextReplacing<float>(filterBuffer2Block);
    
    LP1.process(filterBuffer0Context);
    AP2.process(filterBuffer0Context);
    
    HP1.process(filterBuffer1Context);
    filterBuffers[2] = filterBuffers[1];
    LP2.process(filterBuffer1Context);
    
    HP2.process(filterBuffer2Context);
    
    for ( size_t i = 0; i < filterBuffers.size(); ++i )
        compressors[i].process(filterBuffers[i]);
    
    auto numSamples = buffer.getNumSamples();
    auto numChannels = buffer.getNumChannels();
    
    buffer.clear();
    
    auto addFilterBand = [nc = numChannels, ns = numSamples](auto& inputBuffer, const auto& source)
    {
        for ( auto i = 0; i < nc; ++i )
            inputBuffer.addFrom(i, 0, source, i, 0, ns);
    };
    
    auto bandsAreSoloed = false;
    for ( auto& comp : compressors )
    {
        if ( comp.solo->get() )
        {
            bandsAreSoloed = true;
            break;
        }
    }
    
    if ( bandsAreSoloed )
    {
        for ( size_t i = 0; i < compressors.size(); ++i )
        {
            auto& comp = compressors[i];
            if ( comp.solo->get() && !comp.mute->get() )
                addFilterBand(buffer, filterBuffers[i]);
        }
    }
    else
    {
        for ( size_t i = 0; i < compressors.size(); ++i )
        {
            auto& comp = compressors[i];
            if ( !comp.mute->get() )
                addFilterBand(buffer, filterBuffers[i]);
        }
    }
}

//==============================================================================
bool SkwiezorMBAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SkwiezorMBAudioProcessor::createEditor()
{
    // return new SkwiezorMBAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SkwiezorMBAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SkwiezorMBAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid() )
    {
        apvts.replaceState(tree);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SkwiezorMBAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    
    using namespace juce;
    using namespace Params;
    const auto& params = GetParams();
    
    auto attackReleaseRange = NormalisableRange<float>(0.1, 500, 0.1, 1);
    
    auto choices = std::vector<double>{ 1, 1.5, 2, 3, 4, 5, 6, 8, 10, 15, 20, 50, 100 };
    juce::StringArray sa;
    for ( auto choice : choices)
    {
        sa.add( juce::String(choice, 1) );
    }

    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Threshold_Low_Band), 1}, params.at(Names::Threshold_Low_Band), NormalisableRange<float>(-60, 12, 1, 1), 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Threshold_Mid_Band), 1}, params.at(Names::Threshold_Mid_Band), NormalisableRange<float>(-60, 12, 1, 1), 0));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Threshold_High_Band), 1}, params.at(Names::Threshold_High_Band), NormalisableRange<float>(-60, 12, 1, 1), 0));
        
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Attack_Low_Band), 1}, params.at(Names::Attack_Low_Band), attackReleaseRange, 50));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Attack_Mid_Band), 1}, params.at(Names::Attack_Mid_Band), attackReleaseRange, 50));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Attack_High_Band), 1}, params.at(Names::Attack_High_Band), attackReleaseRange, 50));
    
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Release_Low_Band), 1}, params.at(Names::Release_Low_Band), attackReleaseRange, 250));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Release_Mid_Band), 1}, params.at(Names::Release_Mid_Band), attackReleaseRange, 250));
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Release_High_Band), 1}, params.at(Names::Release_High_Band), attackReleaseRange, 250));
    
    layout.add(std::make_unique<AudioParameterChoice>(juce::ParameterID{params.at(Names::Ratio_Low_Band), 1}, params.at(Names::Ratio_Low_Band), sa, 3));
    layout.add(std::make_unique<AudioParameterChoice>(juce::ParameterID{params.at(Names::Ratio_Mid_Band), 1}, params.at(Names::Ratio_Mid_Band), sa, 3));
    layout.add(std::make_unique<AudioParameterChoice>(juce::ParameterID{params.at(Names::Ratio_High_Band), 1}, params.at(Names::Ratio_High_Band), sa, 3));
    
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Bypass_Low_Band), 1}, params.at(Names::Bypass_Low_Band), false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Bypass_Mid_Band), 1}, params.at(Names::Bypass_Mid_Band), false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Bypass_High_Band), 1}, params.at(Names::Bypass_High_Band), false));
    
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Mute_Low_Band), 1}, params.at(Names::Mute_Low_Band), false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Mute_Mid_Band), 1}, params.at(Names::Mute_Mid_Band), false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Mute_High_Band), 1}, params.at(Names::Mute_High_Band), false));
    
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Solo_Low_Band), 1}, params.at(Names::Solo_Low_Band), false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Solo_Mid_Band), 1}, params.at(Names::Solo_Mid_Band), false));
    layout.add(std::make_unique<AudioParameterBool>(juce::ParameterID{params.at(Names::Solo_High_Band), 1}, params.at(Names::Solo_High_Band), false));
    
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Low_mid_Crossover_Freq), 1}, params.at(Names::Low_mid_Crossover_Freq), NormalisableRange<float>(20, 999, 1, 1), 400));
    
    layout.add(std::make_unique<AudioParameterFloat>(juce::ParameterID{params.at(Names::Mid_high_Crossover_Freq), 1}, params.at(Names::Mid_high_Crossover_Freq), NormalisableRange<float>(1000, 20000, 1, 1), 2000));
    
    

    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SkwiezorMBAudioProcessor();
}
