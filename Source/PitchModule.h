/*
  ==============================================================================

    PitchModule.h
    Created: 31 Aug 2022 8:48:32pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <RubberBandStretcher.h>

//==============================================================================
/*
*/
class PitchModule : public Component,
                    public Slider::Listener,
                    public Button::Listener,
                    public TextEditor::Listener,
                    public AudioSource,
                    public ChangeBroadcaster
{
public:
    //===Constructor/Destructor=====================================================
    PitchModule(AudioSource* myAudioSource);
    ~PitchModule() override;

    //===Component==================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //===AudioSource==================================================================
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    virtual void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    virtual void releaseResources() override;

    //===Slider::Listener===========================================================
    void sliderValueChanged(Slider* slider) override;

    //===TextEditor::Listener===========================================================
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;

    //===Button::Listener===========================================================
    void buttonClicked(Button* button) override;

    //==============================================================================
    void setTimeStretchRatios();
    float getCurrentSpeedFactor();

private:
    //===UI===========================================================================
    Slider pitchSlider;
    Slider speedSlider;
    TextButton resetSlidersButton;
    ToggleButton  keyLockButton;
    TextEditor rangeTextEditor;

    //===AudioSource==================================================================
    AudioSource* myAudioSource;
    int buffersLength;
    AudioBuffer<float>* sourceBuffer;

    RubberBand::RubberBandStretcher* rubberBand;
    float currentSpeedFactor = 1.0;
    float currentPitchFactor = 1.0;

    bool isInKeyLockMode = true;
    float keyLockPitchValue = 1.0;

    float slidersRange = 0.5;



    class rubberBandRatioTimer : public Timer {
    public:
        bool factorChanged = false;
        void timerCallback() override;
        PitchModule* owner;
    } myRubberBandRatioTimer;

    void resetSliders();

    void calcTempoFactor();
    void calcPitchFactor();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchModule)
};
