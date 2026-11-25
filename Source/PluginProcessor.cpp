#include "PluginProcessor.h"
#include "PluginEditor.h"

MyPluginAudioProcessor::MyPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#else
     : treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
}

MyPluginAudioProcessor::~MyPluginAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MyPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("DIST", 1), "Distortion", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("BASS", 1), "Bass Boost", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    return { params.begin(), params.end() };
}

const juce::String MyPluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool MyPluginAudioProcessor::acceptsMidi() const { return false; }
bool MyPluginAudioProcessor::producesMidi() const { return false; }
bool MyPluginAudioProcessor::isMidiEffect() const { return false; }
double MyPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int MyPluginAudioProcessor::getNumPrograms() { return 1; }
int MyPluginAudioProcessor::getCurrentProgram() { return 0; }
void MyPluginAudioProcessor::setCurrentProgram (int index) {}
const juce::String MyPluginAudioProcessor::getProgramName (int index) { return {}; }
void MyPluginAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

void MyPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {}
void MyPluginAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}
#endif

void MyPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float distVal = *treeState.getRawParameterValue("DIST");
    float bassVal = *treeState.getRawParameterValue("BASS");
    int numSamples = buffer.getNumSamples();

    static float lowState[2] = { 0.0f, 0.0f };

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        int chIdx = channel < 2 ? channel : 0;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];

            // BASS BOOST
            float cutoff = 0.05f;
            lowState[chIdx] += cutoff * (input - lowState[chIdx]);
            float bassHarmonics = std::tanh(lowState[chIdx] * (1.0f + bassVal * 4.0f));
            float boostedSignal = input + (bassHarmonics * bassVal * 1.5f);

            // DISTORTION
            float drive = 1.0f + (distVal * 9.0f);
            float drivenSignal = boostedSignal * drive;
            float output = std::tanh(drivenSignal);

            // MAKEUP GAIN
            float autoGain = 1.0f / (1.0f + (distVal * 0.5f) + (bassVal * 0.3f));
            channelData[sample] = output * autoGain;
        }
    }
}

bool MyPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* MyPluginAudioProcessor::createEditor() { return new MyPluginAudioProcessorEditor (*this); }

void MyPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MyPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (treeState.state.getType()))
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyPluginAudioProcessor();
}
