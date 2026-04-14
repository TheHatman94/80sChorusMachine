#include "PluginProcessor.h"
#include "PluginEditor.h"

_80sMachineAudioProcessor::_80sMachineAudioProcessor()
    : delayBuffer(2, 1) // Inizializzazione minima
{}

_80sMachineAudioProcessor::~_80sMachineAudioProcessor() {}

void _80sMachineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = (float)sampleRate;
    
    // Ridotto il buffer a 0.5 secondi (ancora abbondante, ma riduce l'impronta di memoria)
    int delayBufferSize = (int)(sampleRate * 0.5);
    
    delayBuffer.setSize (2, delayBufferSize, false, true);
    delayBuffer.clear();
    
    writeIndex = 0;
    phase = 0.0f;

    toneFilterL.reset();
    toneFilterR.reset();
}

void _80sMachineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const int numSamples = buffer.getNumSamples();

    // Gestione Mono/Muto
    if (totalNumInputChannels >= 1 && totalNumOutputChannels > 1)
    {
        float rmsL = buffer.getRMSLevel(0, 0, numSamples);
        float rmsR = (totalNumInputChannels > 1) ? buffer.getRMSLevel(1, 0, numSamples) : 0.0f;

        if (rmsL > 0.0001f && rmsR < 0.00001f)
        {
            buffer.copyFrom(1, 0, buffer.getReadPointer(0), numSamples);
        }
    }

    for (auto i = 2; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, numSamples);
    }

    const int delayBufLen = delayBuffer.getNumSamples();
    if (delayBufLen <= 0) return;

    float cutoff = 200.0f + (std::pow(juce::jlimit(0.0f, 1.0f, tone), 2.0f) * 18000.0f);
    auto coefficients = juce::IIRCoefficients::makeLowPass(currentSampleRate, cutoff);
    toneFilterL.setCoefficients(coefficients);
    toneFilterR.setCoefficients(coefficients);

    for (int i = 0; i < numSamples; ++i)
    {
        float lfo = std::sin(phase * 2.0f * juce::MathConstants<float>::pi);
        int processChannels = juce::jmin((int)totalNumOutputChannels, (int)delayBuffer.getNumChannels(), 2);

        for (int channel = 0; channel < processChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            float* delayData = delayBuffer.getWritePointer(channel);

            float inputSample = channelData[i];
            delayData[writeIndex] = inputSample;

            float lfoMod = (channel == 0) ? lfo : std::sin((phase + 0.25f) * 2.0f * juce::MathConstants<float>::pi);
            
            // OTTIMIZZAZIONE LATENZA:
            // Delay base ridotto da 25ms a 7ms.
            // 7ms è il "sweet spot" per i chorus: abbastanza per modulare, troppo poco per essere percepito come ritardo.
            float delaySeconds = 0.007f + (lfoMod * depth);
            
            float readPos = (float)writeIndex - (delaySeconds * currentSampleRate);
            
            while (readPos < 0) readPos += (float)delayBufLen;
            while (readPos >= (float)delayBufLen) readPos -= (float)delayBufLen;

            int p1 = (int)readPos;
            int p2 = (p1 + 1) % delayBufLen;
            
            p1 = juce::jlimit(0, delayBufLen - 1, p1);
            p2 = juce::jlimit(0, delayBufLen - 1, p2);

            float frac = readPos - (float)p1;
            float delayedSample = delayData[p1] * (1.0f - frac) + delayData[p2] * frac;

            if (channel == 0) delayedSample = toneFilterL.processSingleSampleRaw(delayedSample);
            else              delayedSample = toneFilterR.processSingleSampleRaw(delayedSample);

            channelData[i] = (inputSample * (1.0f - mix)) + (delayedSample * mix);
        }

        if (++writeIndex >= delayBufLen) writeIndex = 0;
        
        phase += rate / currentSampleRate;
        if (phase >= 1.0f) phase -= 1.0f;
    }
}

void _80sMachineAudioProcessor::releaseResources() {}

juce::AudioProcessorEditor* _80sMachineAudioProcessor::createEditor()
{
    return new _80sMachineAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new _80sMachineAudioProcessor();
}
