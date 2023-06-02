/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicCompAudioProcessorEditor::BasicCompAudioProcessorEditor(BasicCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    panDial.setLookAndFeel(&otherLookAndFeel);
    panDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    panDial.setTextBoxStyle(juce::Slider::NoTextBox, false, big, 50);
    panDial.setDoubleClickReturnValue(true, 0.0);
    addAndMakeVisible(panDial);
    addAndMakeVisible(panDialLabel);
    panDialLabel.setText("Pan", juce::dontSendNotification);
    panDialLabel.setJustificationType(juce::Justification::horizontallyCentred);
    panDialLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    panDialLabel.setLookAndFeel(&otherLookAndFeel);
    panDialLabel.attachToComponent(&panDial, false);
    panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "panner", panDial);

    gainFader.setLookAndFeel(&otherLookAndFeel);
    gainFader.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gainFader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, big, 25);
    gainFader.setNumDecimalPlacesToDisplay(2);
    gainFader.setDoubleClickReturnValue(true, 0.0);
    addAndMakeVisible(gainFader);
    addAndMakeVisible(gainFaderLabel);
    gainFaderLabel.setText("Gain", juce::dontSendNotification);
    gainFaderLabel.setJustificationType(juce::Justification::horizontallyCentred);
    gainFaderLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    gainFaderLabel.setLookAndFeel(&otherLookAndFeel);
    gainFaderLabel.attachToComponent(&gainFader, false);
    faderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "fader", gainFader);


    compInput.setLookAndFeel(&otherLookAndFeel);
    compInput.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compInput.setTextBoxStyle(juce::Slider::TextBoxBelow, false, small, 25);
    compInput.setDoubleClickReturnValue(true, 0.0);
    compInput.setTextValueSuffix(" dB");
    addAndMakeVisible(compInput);
    addAndMakeVisible(compInputLabel);
    compInputLabel.setText("Input", juce::dontSendNotification);
    compInputLabel.setJustificationType(juce::Justification::horizontallyCentred);
    compInputLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    compInputLabel.setLookAndFeel(&otherLookAndFeel);
    compInputLabel.attachToComponent(&compInput, false);
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "input", compInput);


    compThresh.setLookAndFeel(&otherLookAndFeel);
    compThresh.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compThresh.setTextBoxStyle(juce::Slider::TextBoxBelow, false, small, 25);
    compThresh.setDoubleClickReturnValue(true, 0.0);
    compThresh.setTextValueSuffix(" dB");
    addAndMakeVisible(compThresh);
    addAndMakeVisible(compThreshLabel);
    compThreshLabel.setText("Thresh", juce::dontSendNotification);
    compThreshLabel.setJustificationType(juce::Justification::horizontallyCentred);
    compThreshLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    compThreshLabel.setLookAndFeel(&otherLookAndFeel);
    compThreshLabel.attachToComponent(&compThresh, false);
    threshAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "thresh", compThresh);


    compRatio.setLookAndFeel(&otherLookAndFeel);
    compRatio.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compRatio.setTextBoxStyle(juce::Slider::TextBoxBelow, false, small, 25);
    compRatio.setDoubleClickReturnValue(true, 1.0);
    addAndMakeVisible(compRatio);
    addAndMakeVisible(compRatioLabel);
    compRatioLabel.setText("Ratio", juce::dontSendNotification);
    compRatioLabel.setJustificationType(juce::Justification::horizontallyCentred);
    compRatioLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    compRatioLabel.setLookAndFeel(&otherLookAndFeel);
    compRatioLabel.attachToComponent(&compRatio, false);
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "ratio", compRatio);


    compAttack.setLookAndFeel(&otherLookAndFeel);
    compAttack.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compAttack.setTextBoxStyle(juce::Slider::TextBoxBelow, false, small, 25);
    compAttack.setDoubleClickReturnValue(true, 10.0);
    compAttack.setTextValueSuffix(" ms");
    addAndMakeVisible(compAttack);
    addAndMakeVisible(compAttackLabel);
    compAttackLabel.setText("Attack", juce::dontSendNotification);
    compAttackLabel.setJustificationType(juce::Justification::horizontallyCentred);
    compAttackLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    compAttackLabel.setLookAndFeel(&otherLookAndFeel);
    compAttackLabel.attachToComponent(&compAttack, false);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "attack", compAttack);



    compRelease.setLookAndFeel(&otherLookAndFeel);
    compRelease.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compRelease.setTextBoxStyle(juce::Slider::TextBoxBelow, false, small, 25);
    compRelease.setDoubleClickReturnValue(true, 125.0);
    compRelease.setTextValueSuffix(" ms");
    addAndMakeVisible(compRelease);
    addAndMakeVisible(compReleaseLabel);
    compReleaseLabel.setText("Release", juce::dontSendNotification);
    compReleaseLabel.setJustificationType(juce::Justification::horizontallyCentred);
    compReleaseLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    compReleaseLabel.setLookAndFeel(&otherLookAndFeel);
    compReleaseLabel.attachToComponent(&compRelease, false);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "release", compRelease);



    compOutput.setLookAndFeel(&otherLookAndFeel);
    compOutput.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compOutput.setTextBoxStyle(juce::Slider::TextBoxBelow, false, small, 25);
    compOutput.setDoubleClickReturnValue(true, 0.0);
    compOutput.setTextValueSuffix(" dB");
    addAndMakeVisible(compOutput);
    addAndMakeVisible(compOutputLabel);
    compOutputLabel.setText("Output", juce::dontSendNotification);
    compOutputLabel.setJustificationType(juce::Justification::horizontallyCentred);
    compOutputLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    compOutputLabel.setLookAndFeel(&otherLookAndFeel);
    compOutputLabel.attachToComponent(&compOutput, false);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "output", compOutput);

    setSize(500, 600);
}


BasicCompAudioProcessorEditor::~BasicCompAudioProcessorEditor()
{
}

//==============================================================================
void BasicCompAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto wood = juce::ImageCache::getFromMemory(BinaryData::figured_maple_jpeg, BinaryData::figured_maple_jpegSize);
    g.drawImage(wood, getLocalBounds().toFloat());
}

void BasicCompAudioProcessorEditor::resized()
{
    float gapX{ (getWidth() - (small + small + big)) / 3 };
    float gapY1{ (getHeight()-(small*3))/3 };


    panDial.setBounds((gapX*2.4 + small*2), (getHeight() / 3) - 100, big, big);

    gainFader.setBounds((gapX*2.4 + small*2), (getHeight() / 3) + 125, big, 137);

    compInput.setBounds((gapX / 2), (small*.67), small, small+25);

    compThresh.setBounds((gapX/2), (getHeight() / 3) + 25, small, small+25);

    compRatio.setBounds((gapX/2), (getHeight() / 3) + 200, small, small+25);

    compAttack.setBounds(((getWidth() / 2) - small * 2 / 3), (small * .67), small, small+25);

    compRelease.setBounds((getWidth() / 2) - small * 2 / 3, (getHeight() / 3) + 25, small, small+25);

    compOutput.setBounds((getWidth() / 2) - small * 2 / 3, (getHeight() / 3) + 200, small, small+25);
}

