#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class _80sMachineAudioProcessor  : public juce::AudioProcessor
{
public:
    _80sMachineAudioProcessor();
    ~_80sMachineAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "80s Chorus Machine"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

    // Parametri pubblici accessibili dall'editor
    float rate = 0.5f;
    float depth = 0.002f;
    float mix = 0.5f;
    float tone = 1.0f;

private:
    // Buffer per il delay del chorus
    juce::AudioSampleBuffer delayBuffer;
    int writeIndex = 0;
    float phase = 0.0f;
    float currentSampleRate = 44100.0f;

    // Filtro per il controllo del tono
    juce::IIRFilter toneFilterL, toneFilterR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_80sMachineAudioProcessor)
};
