#include "PluginProcessor.h"
#include "PluginEditor.h"

_80sMachineAudioProcessorEditor::_80sMachineAudioProcessorEditor (_80sMachineAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (500, 300);

    // Setup dei colori per gli slider
    auto setupNeonSlider = [this](juce::Slider& s, juce::Label& l, juce::String name, juce::Colour neonColor) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        
        // Colore del pomello
        s.setColour(juce::Slider::rotarySliderFillColourId, neonColor);
        s.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        
        s.addListener(this);
        addAndMakeVisible(s);

        l.setText(name, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setColour(juce::Label::textColourId, neonColor);
        l.attachToComponent(&s, false);
        addAndMakeVisible(l);
    };

    setupNeonSlider(rateSlider, rateLabel, "RATE", juce::Colours::cyan);
    rateSlider.setRange(0.05, 2.0);
    rateSlider.setValue(audioProcessor.rate);

    setupNeonSlider(depthSlider, depthLabel, "DEPTH", juce::Colours::magenta);
    depthSlider.setRange(0.0001, 0.005);
    depthSlider.setValue(audioProcessor.depth);

    setupNeonSlider(toneSlider, toneLabel, "TONE", juce::Colour(0xff00ff00)); // Verde Neon
    toneSlider.setRange(0.0, 1.0);
    toneSlider.setSkewFactor(0.5);
    toneSlider.setValue(audioProcessor.tone);

    // Mix Slider orizzontale
    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    mixSlider.setColour(juce::Slider::trackColourId, juce::Colours::cyan);
    mixSlider.setColour(juce::Slider::thumbColourId, juce::Colours::magenta);
    mixSlider.setRange(0.0, 1.0);
    mixSlider.setValue(audioProcessor.mix);
    mixSlider.addListener(this);
    addAndMakeVisible(mixSlider);
}

_80sMachineAudioProcessorEditor::~_80sMachineAudioProcessorEditor() {}

void _80sMachineAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &rateSlider)  audioProcessor.rate  = (float)rateSlider.getValue();
    if (slider == &depthSlider) audioProcessor.depth = (float)depthSlider.getValue();
    if (slider == &mixSlider)   audioProcessor.mix   = (float)mixSlider.getValue();
    if (slider == &toneSlider)  audioProcessor.tone  = (float)toneSlider.getValue();
}

void _80sMachineAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Sfondo con gradiente
    juce::ColourGradient backgroundGradient (juce::Colour(0xff0f0524), (float)getWidth()/2, (float)getHeight()/2,
                                             juce::Colours::black, 0.0f, (float)getHeight(), true);
    g.setGradientFill(backgroundGradient);
    g.fillAll();

    g.setColour(juce::Colours::magenta.withAlpha(0.2f));
    for (int x = 0; x < getWidth(); x += 40)
        g.drawLine((float)x, 0.0f, (float)x, (float)getHeight(), 0.5f);
    for (int y = 0; y < getHeight(); y += 40)
        g.drawLine(0.0f, (float)y, (float)getWidth(), (float)y, 0.5f);

    g.setFont (juce::Font ("Arial", 26.0f, juce::Font::bold | juce::Font::italic));
    
    // Ombra
    g.setColour(juce::Colours::magenta);
    g.drawText ("80s CHORUS MACHINE", 2, 12, getWidth(), 40, juce::Justification::centred);
    
    // Testo sopra
    g.setColour(juce::Colours::cyan);
    g.drawText ("80s CHORUS MACHINE", 0, 10, getWidth(), 40, juce::Justification::centred);

    // cornice 
    g.setColour(juce::Colours::cyan.withAlpha(0.5f));
    g.drawRoundedRectangle(10, 50, getWidth() - 20, getHeight() - 100, 10.0f, 2.0f);
}

void _80sMachineAudioProcessorEditor::resized()
{
    auto r = getLocalBounds().reduced(20);
    r.removeFromTop(60);
    
    auto topArea = r.removeFromTop(120);
    int knobWidth = topArea.getWidth() / 3;
    
    rateSlider.setBounds(topArea.removeFromLeft(knobWidth).reduced(10));
    depthSlider.setBounds(topArea.removeFromLeft(knobWidth).reduced(10));
    toneSlider.setBounds(topArea.reduced(10));

    mixSlider.setBounds(50, 260, getWidth() - 100, 30);
}
