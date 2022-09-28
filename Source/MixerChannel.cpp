/*
  ==============================================================================

    MixerChannel.cpp
    Created: 5 May 2020 4:50:47pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MixerChannel.h"

//==============================================================================
MixerChannel::MixerChannel () :
    channelColour (Colours::white),
    sFader (Slider::LinearVertical, Slider::NoTextBox),
    sGain (Slider::RotaryVerticalDrag, Slider::NoTextBox),
    sFilter (Slider::RotaryVerticalDrag, Slider::NoTextBox),
    sPan (Slider::RotaryVerticalDrag, Slider::NoTextBox),
    mySoundSource(NULL),
    crossFaderValue(1.0),
    volumeFactor(0.0),
    m_pAudioBuffer(NULL)
{
    addAndMakeVisible (&sFader);
    sFader.setRange (0.0, 1.0);
    sFader.addListener (this);
#ifdef  DEBUG
    sFader.setValue(1.0);
#endif //  DEBUG


    addAndMakeVisible (&sGain);
    sGain.setRange (0.0, 5.0);
    sGain.setSkewFactorFromMidPoint (1.0);
    sGain.setValue (1.0);
    sGain.addListener (this);

    addAndMakeVisible (&sFilter);
    sFilter.setRange (-1.0f, 1.0f);
    sFilter.setValue (0.0f);
    sFilter.addListener (this);

    addAndMakeVisible (&sPan);
    sPan.setRange (-1.0f, 1.0f);
    sPan.setValue (0.0f);
    sPan.addListener (this);
    setPanFact ();
}

MixerChannel::~MixerChannel()
{
}

//==============================================================================
void MixerChannel::sliderValueChanged (Slider* slider)
{
    if (slider == &sFader || slider == &sGain)
    {
        setVolume ();
    }

    if (slider == &sFilter)
    {
        updateFilter ();
    }

    if (slider == &sPan)
    {
        setPanFact ();
    }
}

//==============================================================================
void MixerChannel::paint (Graphics& g)
{
}

void MixerChannel::resized()
{
    sGain.setBounds (getWidth() / 2 - 28, 0, 56, 56);
    sFilter.setBounds (getWidth () / 2 - 35, 56, 70, 70);
    sPan.setBounds (getWidth () / 2 - 23, 126, 46, 46);
    sFader.setBounds (0, 172, getWidth(), getHeight() - 172);
}

//==============================================================================
void MixerChannel::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    this->sampleRate = sampleRate;

    m_pAudioBuffer = new AudioBuffer<float> (2, samplesPerBlockExpected);
    mySoundSource->prepareToPlay (samplesPerBlockExpected, sampleRate);

    // Preparing filter
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 2;

    filter_stateVariable.prepare (spec);
    updateFilter ();
}

void MixerChannel::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    float* const leftSpeakerOut = bufferToFill.buffer->getWritePointer (0, bufferToFill.startSample);
    float* const rightSpeakerOut = bufferToFill.buffer->getWritePointer (1, bufferToFill.startSample);

    AudioSourceChannelInfo t_mixBuffer (m_pAudioBuffer, 0, bufferToFill.numSamples);

    // Read music stream from buffer;
    mySoundSource->getNextAudioBlock (t_mixBuffer);

    if (filterUsed)
    {
        dsp::AudioBlock<float> block (*t_mixBuffer.buffer);
        filter_stateVariable.process (dsp::ProcessContextReplacing<float> (block));
    }

    float* const leftSpeakerIn = t_mixBuffer.buffer->getWritePointer (0, t_mixBuffer.startSample);
    float* const rightSpeakerIn = t_mixBuffer.buffer->getWritePointer (1, t_mixBuffer.startSample);

    // Copy from in buffer to out buffer with respect to the master
    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        leftSpeakerOut[sample] = leftSpeakerIn[sample] * volumeFactor * panFactorL;
        rightSpeakerOut[sample] = rightSpeakerIn[sample] * volumeFactor * panFactorR;
    }
}

void MixerChannel::releaseResources ()
{
    delete m_pAudioBuffer;
}

//==============================================================================
void MixerChannel::attachvJDSoundSource (vJDSoundSource* newSoundSource)
{
    mySoundSource = newSoundSource;

    channelColour = newSoundSource->getColour ();

    sFader.setColour (Slider::thumbColourId, channelColour);
}

//==============================================================================
void MixerChannel::setCrossFaderValue (float newVal)
{
    crossFaderValue = newVal;
    setVolume ();
}

void MixerChannel::setVolume ()
{
    volumeFactor = sFader.getValue () * sGain.getValue () * crossFaderValue;
}

void MixerChannel::setPanFact ()
{
    if (sPan.getValue () < -0.1)
        panFactorL = 1.0f;
    else
        panFactorL = (-10 * sPan.getValue () + 10) / 11;

    if (sPan.getValue () > 0.1)
        panFactorR = 1.0f;
    else
        panFactorR = (10 * sPan.getValue () + 10) / 11;
}

void MixerChannel::updateFilter ()
{
    // If filter slider is in neutral stand: Turn of filter.
    if (sFilter.getValue() == 0.0f)
    {
        filterUsed = false;
        return;
    }

    filterUsed = true;
    // High-pass filter
    if (sFilter.getValue() > 0.0f)
    {
        filter_stateVariable.state->type = dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        // 0.0f -> 20Hz; 1.0f -> 20.000Hz3
        filter_stateVariable.state->setCutOffFrequency (sampleRate,
            19 + pow(19981, sFilter.getValue()), 1.0 / MathConstants<float>::sqrt2);
    }

    // Low-pass filter
    else
    {
        filter_stateVariable.state->type = dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
        // 0.0f-> 20.000Hz; -1.0f -> 20Hz
        filter_stateVariable.state->setCutOffFrequency (sampleRate,
            19 + pow(19981, sFilter.getValue() + 1.0), 1.0 / MathConstants<float>::sqrt2);
    }
}