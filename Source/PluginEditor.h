#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MyPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MyPluginAudioProcessorEditor (MyPluginAudioProcessor&);
    ~MyPluginAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MyPluginAudioProcessor& audioProcessor;

    juce::Image bgImage;
    juce::Slider distSlider;
    juce::Slider bassSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyPluginAudioProcessorEditor)
};
