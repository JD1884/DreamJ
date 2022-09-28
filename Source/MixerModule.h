/*
  ==============================================================================

    MixerModule.h
    Created: 5 May 2020 4:51:06pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "MixerChannel.h"
#include "CrossFader.h"

#include "vJDSoundSource.h"

constexpr auto NCHANNELS = 2;

//==============================================================================
/*
*/
class MixerModule    : public Component,
                        public AudioSource,
                        public Slider::Listener
                        
{
public:
    MixerModule();
    ~MixerModule();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    virtual void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    virtual void releaseResources() override;

    //==============================================================================
    void sliderValueChanged (Slider* slider) override;

    //==============================================================================
    void attachVJDSoundSource (vJDSoundSource* newSoundSource, int channel);
    void saveState ();

private:
    // Mixer module to combine the diffrent channels in one audio stream.
    MixerAudioSource mixer;

    AudioBuffer<float>* m_pAudioBuffer;

    MixerChannel channels[NCHANNELS];
    CrossFader crossFader;
    Slider master;
    float masterValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MixerModule)
};
