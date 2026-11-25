#include "PluginProcessor.h"
#include "PluginEditor.h"

MyPluginAudioProcessorEditor::MyPluginAudioProcessorEditor (MyPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Если картинка называется my_photo.png, замени ниже _jpg на _png в обоих местах
    bgImage = juce::ImageCache::getFromMemory(BinaryData::my_photo_jpg, BinaryData::my_photo_jpgSize);

    distSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(distSlider);
    distAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, "DIST", distSlider);

    bassSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(bassSlider);
    bassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, "BASS", bassSlider);

    setSize (800, 500);
}

MyPluginAudioProcessorEditor::~MyPluginAudioProcessorEditor() {}

void MyPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    if (bgImage.isValid())
        g.drawImage (bgImage, getLocalBounds().toFloat());
    else
        g.fillAll (juce::Colours::darkgrey);
}

void MyPluginAudioProcessorEditor::resized()
{
    distSlider.setBounds(150, 300, 120, 120);
    bassSlider.setBounds(550, 300, 120, 120);
}
