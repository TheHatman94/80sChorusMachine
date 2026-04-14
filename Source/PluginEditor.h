#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class _80sMachineAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          public juce::Slider::Listener
{
public:
    _80sMachineAudioProcessorEditor (_80sMachineAudioProcessor&);
    ~_80sMachineAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    _80sMachineAudioProcessor& audioProcessor;

    juce::Slider rateSlider, depthSlider, mixSlider, toneSlider;
    juce::Label  rateLabel, depthLabel, mixLabel, toneLabel;

    void setupSlider(juce::Slider& s, juce::Label& l, juce::String name);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_80sMachineAudioProcessorEditor)
};
