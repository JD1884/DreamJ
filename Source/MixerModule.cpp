/*
  ==============================================================================

    MixerModule.cpp
    Created: 5 May 2020 4:51:06pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MixerModule.h"

//==============================================================================
MixerModule::MixerModule():
    master(Slider::Rotary, Slider::NoTextBox),
    m_pAudioBuffer(NULL)
{
    for (int i = 0; i < NCHANNELS; i++)
        addAndMakeVisible (&channels[i]);

    addAndMakeVisible (crossFader);

    addAndMakeVisible (master);
    master.setRange (0.0, 2.0);
    master.setValue (1.0);
    master.addListener (this);
    masterValue = 1.0f;
}

MixerModule::~MixerModule()
{
}


//==============================================================================
void MixerModule::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::white);
    g.drawRect (getLocalBounds(), 2);   // draw an outline around the component
}

void MixerModule::resized()
{
    master.setBounds (getWidth () - 50, 0, 50, 50);

    for (int i = 0; i < NCHANNELS; i++)
    {
        channels[i].setBounds (i * getWidth () / NCHANNELS + 5, 50,
            getWidth () / NCHANNELS - 10, getHeight () - 110);
    }

    crossFader.setBounds (5, getHeight () - 50, getWidth () - 10, 50);
}

//==============================================================================
void MixerModule::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    mixer.prepareToPlay (samplesPerBlockExpected, sampleRate);
    // Initialize audioBuffer
    m_pAudioBuffer = new AudioBuffer<float> (2, samplesPerBlockExpected);
}

void  MixerModule::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    float* const leftSpeakerOut = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    float* const rightSpeakerOut = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);

    AudioSourceChannelInfo t_mixBuffer(m_pAudioBuffer, 0, bufferToFill.numSamples);

    // Read music stream from buffer;
    mixer.getNextAudioBlock (t_mixBuffer);

    float* const leftSpeakerIn = t_mixBuffer.buffer->getWritePointer (0, t_mixBuffer.startSample);
    float* const rightSpeakerIn = t_mixBuffer.buffer->getWritePointer (1, t_mixBuffer.startSample);

    // Copy from in buffer to out buffer with respect to the master
    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        leftSpeakerOut[sample] = leftSpeakerIn[sample] * masterValue;
        rightSpeakerOut[sample] = rightSpeakerIn[sample] * masterValue;
    }
}

void  MixerModule::releaseResources ()
{
    // Releases mixer resources autoamtically releases all the attached channel resources. No need for releasing them sepperattly.
    mixer.releaseResources ();

    delete m_pAudioBuffer;
}

//==============================================================================
void MixerModule::sliderValueChanged (Slider* slider)
{
    if (slider == &master)
    {
        // Save new master value
        masterValue = slider->getValue ();
    }
}

//==============================================================================
void MixerModule::attachVJDSoundSource (vJDSoundSource* newSoundSource, int channel)
{
    

    if (channel > 0 && channel <= NCHANNELS)
    {
        channels[channel - 1].attachvJDSoundSource(newSoundSource);
        mixer.addInputSource (&channels[channel - 1], false);
    }

    crossFader.attachChannel (&channels[channel - 1], channel);

    crossFader.setBounds (1, 1, 1, 1);
    crossFader.setBounds (5, getHeight () - 50, getWidth () - 10, 50);
}

void MixerModule::saveState ()
{
    crossFader.saveState ();
}