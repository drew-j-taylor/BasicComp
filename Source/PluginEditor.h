/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <cmath>
#include <math.h>

//==============================================================================
/**
*/

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
        setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(67u, 67u, 67u));
        setColour(juce::Label::textColourId, juce::Colours::black);
    }
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override {
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        // fill
        //g.setColour(juce::Colour(45u, 45u, 45u));
        g.setColour(juce::Colour(40u, 40u, 40u));
        g.fillEllipse(rx, ry, rw, rw);

        // outline
        g.setColour(juce::Colours::black);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);
        juce::Path p;
        auto pointerLength = radius * 0.625f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }

};

class BasicCompAudioProcessorEditor : public juce::AudioProcessorEditor
    //,public juce::Slider::Listener
{
public:
    BasicCompAudioProcessorEditor(BasicCompAudioProcessor&);
    ~BasicCompAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    //void sliderValueChanged(juce::Slider* slider) override;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicCompAudioProcessor& audioProcessor;
    OtherLookAndFeel otherLookAndFeel;

    float small{ 75 }, big{ 112 };

    juce::Slider panDial;
    juce::Label panDialLabel;
    std::unique_ptr<SliderAttachment> panAttachment;

    juce::Slider gainFader;
    juce::Label gainFaderLabel;
    std::unique_ptr<SliderAttachment> faderAttachment;

    juce::Slider compInput;
    juce::Label compInputLabel;
    std::unique_ptr<SliderAttachment> inputAttachment;

    juce::Slider compThresh;
    juce::Label compThreshLabel;
    std::unique_ptr<SliderAttachment> threshAttachment;

    juce::Slider compAttack;
    juce::Label compAttackLabel;
    std::unique_ptr<SliderAttachment> attackAttachment;

    juce::Slider compRatio;
    juce::Label compRatioLabel;
    std::unique_ptr<SliderAttachment> ratioAttachment;

    juce::Slider compRelease;
    juce::Label compReleaseLabel;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    juce::Slider compOutput;
    juce::Label compOutputLabel;
    std::unique_ptr<SliderAttachment> outputAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicCompAudioProcessorEditor)
};

