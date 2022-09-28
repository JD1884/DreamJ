/*
  ==============================================================================

    MixerChannel.h
    Created: 5 May 2020 4:50:47pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "vJDSoundSource.h"

//==============================================================================
/*
*/
class MixerChannel    : public Component,
                        public AudioSource,
                        public Slider::Listener
{
public:
    //===Constructor/Destructor=====================================================
    MixerChannel();
    ~MixerChannel();

    //===Component==================================================================
    void paint (Graphics&) override;
    void resized() override;

    //===AudioSource================================================================
    virtual void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    virtual void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    virtual void releaseResources () override;

    //===Slider::Listener===========================================================
    void sliderValueChanged (Slider* slider) override;

    //==============================================================================
    void attachvJDSoundSource (vJDSoundSource* newSoundSource);

    void setCrossFaderValue (float newVal);

    Colour getColour () { return channelColour; };

private:
    // Audio buffer to process incomming audio
    AudioBuffer<float>* m_pAudioBuffer;

    Slider sFader;
    Slider sGain;
    float crossFaderValue;
    float volumeFactor;

    float panFactorR;
    float panFactorL;

    Slider sFilter;
    dsp::ProcessorDuplicator <dsp::StateVariableFilter::Filter<float>, dsp::StateVariableFilter::Parameters<float>>
        filter_stateVariable;
    bool filterUsed;

    Slider sPan;

    Colour channelColour;

    // To do, make this a vector, ... so we can attach more devices.
    vJDSoundSource* mySoundSource;

    void setVolume ();
    void setPanFact ();
    void updateFilter ();
    
    double sampleRate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerChannel)
};
