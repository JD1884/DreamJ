/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent () :
    musicPlayerA ("Left Deck", Colours::green, &formatManager),
    musicPlayerB ("Right Deck", Colours::red, &formatManager),
    wavDisplayZoomer (Slider::LinearVertical, Slider::NoTextBox)
{
    // Audio settings
    setAudioChannels (0, 2);

    addAndMakeVisible (&musicPlayerA);
    musicPlayerA.attachWavDisplay (&musicPlayerAVisualiser);
    addAndMakeVisible (musicPlayerAVisualiser);
    addAndMakeVisible (&musicPlayerB);
    musicPlayerB.attachWavDisplay (&musicPlayerBVisualiser);
    addAndMakeVisible (musicPlayerBVisualiser);

    mixerModule.attachVJDSoundSource (&musicPlayerA, 1);
    mixerModule.attachVJDSoundSource (&musicPlayerB, 2);

    addAndMakeVisible (&mixerModule);

    addAndMakeVisible (theBrowser);
    theBrowser->addActionListener (this);

    formatManager.registerBasicFormats ();


    ValueTree mainSettings = theSettingsManager->getSpecificSettings (setIDs::mainID);
    addAndMakeVisible (wavDisplayZoomer);
    wavDisplayZoomer.setRange (0.5f, 5.0f);
    wavDisplayZoomer.setValue (2.0f);
    wavDisplayZoomer.addListener (this);
    if (mainSettings.hasProperty (mainIDs::wavDisplayZoomerVal))
        wavDisplayZoomer.setValue (mainSettings.getProperty (mainIDs::wavDisplayZoomerVal));

    // Graphic settings
    setSize (1000, 750);
}

MainComponent::~MainComponent()
{
    mixerModule.saveState ();

    saveState ();

    delete theBrowser;
    delete theMusicDataBase;
    delete theSettingsManager;

    // This shuts down the audio device and clears the audio source.
    shutdownAudio ();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    mixerModule.prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerModule.getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    mixerModule.releaseResources ();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colour::fromFloatRGBA (1.0f, 1.0f, 1.0f, 0.5f));
    g.drawLine (getWidth () / 2, 2 * getHeight () / 3 - 70, getWidth () / 2, 2 * getHeight () / 3 + 70);
}

void MainComponent::resized()
{
    musicPlayerA.setBounds (5, 5, 4 * getWidth () / 11 - 10, 2 * getHeight () / 3 - 80);
    mixerModule.setBounds (4 * getWidth () / 11 + 5, 5, 3 * getWidth () / 11 - 10, 2 * getHeight () / 3 - 80);
    musicPlayerB.setBounds (7 * getWidth () / 11 + 5, 5, 4 * getWidth () / 11 - 10, 2 * getHeight () / 3 - 80);

    musicPlayerAVisualiser.setBounds (0, 2 * getHeight () / 3 - 70, getWidth(), 70);
    musicPlayerBVisualiser.setBounds (0, 2 * getHeight () / 3, getWidth (), 70);

    wavDisplayZoomer.setBounds (getWidth() - 50, 2 * getHeight () / 3 - 60, 30, 120);

    theBrowser->setBounds (0, 2 * getHeight () / 3 + 70, getWidth (), getHeight () / 3 - 70);

    repaint ();
}

//==============================================================================
void MainComponent::sliderValueChanged (Slider* slider)
{
    if (slider = &wavDisplayZoomer)
    {
        musicPlayerAVisualiser.setRange (wavDisplayZoomer.getValue());
        musicPlayerBVisualiser.setRange (wavDisplayZoomer.getValue ());
    }
}

// Called by the browser when dubbel clicked on a file
void MainComponent::actionListenerCallback (const String& message)
{
    if (!musicPlayerA.isPlaying () || !musicPlayerB.isPlaying ())
    {
        if (musicPlayerA.isPlaying ())
            musicPlayerB.load (File (message));
        else if (musicPlayerB.isPlaying ())
            musicPlayerA.load (File (message));
        else
        {
            // Check wicht music player has the oldest loaded file.
            if (musicPlayerA.getLastLoad () > musicPlayerB.getLastLoad ())
                musicPlayerB.load (File (message));
            else
                musicPlayerA.load (File (message));
        }
    }
}

void MainComponent::saveState ()
{
    ValueTree mainSettings = theSettingsManager->getSpecificSettings (setIDs::mainID);

    mainSettings.setProperty (mainIDs::wavDisplayZoomerVal, wavDisplayZoomer.getValue (), nullptr);
}