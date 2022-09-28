/*==============================================================================

    vJDSoundSource.h
    Created: 5 May 2020 5:30:45pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class vJDSoundSource    :   public AudioSource,
                            public Component
{
public:
    vJDSoundSource () {}
    ~vJDSoundSource () {}

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override = 0;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override = 0;
    void releaseResources () override = 0;

    //==============================================================================
    void paint (Graphics&) override = 0;
    void resized () override = 0;

    //==============================================================================
    virtual Colour getColour () { return Colours::white; };

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (vJDSoundSource)
};
