/*
  ==============================================================================

    MusicPlayer.cpp
    Created: 5 May 2020 1:29:28pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MusicPlayer.h"

//===Constructor/Destructor=====================================================
MusicPlayer::MusicPlayer(String tName, Colour tColour, AudioFormatManager* p_formatManager) :
    playerName(tName),
    playerColour(tColour),
    state(sIdle),
    LabelmusicInfo("LabelMusicInfo", " "),
    loadButton("Load"),
    playButton("Play"),
    stopButton("Stop"),
    formatManager(p_formatManager),
    m_visualiser(p_formatManager),
    visCursor(transport),
    m_hotCueOverlay(transport),
    m_hotCue(transport, m_hotCueOverlay),
    lastLoad (0),
    pitchModule(&transport)
{
    // Component settings
    addAndMakeVisible (&LabelmusicInfo);
    LabelmusicInfo.setEditable (false, false, false);

    addAndMakeVisible (&loadButton);
    loadButton.onClick = [this] { clickLoadButton (); };

    addAndMakeVisible (&playButton);
    playButton.onClick = [this] { clickPlayButton (); };
    playButton.setColour (TextButton::buttonColourId, Colours::green);
    playButton.setTriggeredOnMouseDown (true);

    addAndMakeVisible (&stopButton);
    stopButton.onClick = [this] { clickStopButton (); };
    stopButton.setColour (TextButton::buttonColourId, Colours::red);
    stopButton.setTriggeredOnMouseDown (true);

    addAndMakeVisible (&m_visualiser);

    addAndMakeVisible (m_hotCue);
    addAndMakeVisible (m_hotCueOverlay);

    // Be sure to add this after m_visualiser and m_hotCueOverlay to ensure the scroll function.
    addAndMakeVisible (visCursor);

    transport.addChangeListener (this);
    pitchModule.addChangeListener (this);

    // After initialization: turn music player to empty
    changeState (sEmpty);

    addAndMakeVisible(pitchModule);

    startTimer (20);
}

MusicPlayer::~MusicPlayer()
{
    transport.stop ();
}

//===AudioSource================================================================
void MusicPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    //tempoManager.prepareToPlay(samplesPerBlockExpected, sampleRate);
    transport.prepareToPlay (samplesPerBlockExpected, sampleRate);
    pitchModule.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MusicPlayer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    currentPosition = transport.getCurrentPosition ();

    if (playerSource.get () == nullptr)
    {
        bufferToFill.clearActiveBufferRegion ();
        return;
    }

    /*if (!isPlaying())
        return;*/
    
    pitchModule.getNextAudioBlock (bufferToFill);
}

void MusicPlayer::releaseResources ()
{
    pitchModule.releaseResources();
    transport.releaseResources();
}

//===Component==================================================================
void MusicPlayer::paint (Graphics& g)
{


    g.fillAll (getLookAndFeel ().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (playerColour);
    g.drawRect (getLocalBounds (), 2);   // draw an outline around the component
}

void MusicPlayer::resized ()
{
    LabelmusicInfo.setBounds (10, 10, 0.66 * getWidth () - 20, 30);
    loadButton.setBounds (10, 50, 0.66 * getWidth () - 20, 30);
    playButton.setBounds (10, 90, 0.66 * getWidth () - 20, 30);
    stopButton.setBounds (10, 130, 0.66 * getWidth () - 20, 30);

    Rectangle<int> thumbnailRect (10, 170, 0.66 * getWidth () - 20, 40);
    m_visualiser.setBounds (thumbnailRect); 
    visCursor.setBounds (thumbnailRect);

    m_hotCue.setBounds (10, 220, 0.66 * getWidth () - 20, 100);
    m_hotCueOverlay.setBounds (thumbnailRect);

    pitchModule.setBounds(0.66 * getWidth(), 0, 0.34 * getWidth(), getHeight());
}

void MusicPlayer::attachWavDisplay (WavDisplay* theWavDisplay)
{
    m_wavDisplay = theWavDisplay;

    m_wavDisplay->setup (formatManager, playerColour, &transport);
}

//==============================================================================
void MusicPlayer::load (File mFile)
{
    if (state != sEmpty)
        // Save info of previous track (also on failed load attempt, doesn't matter)
        saveTrackInfo ();


    musicFile = mFile;

    AudioFormatReader* tempReader = formatManager->createReaderFor (musicFile);

    if (tempReader != nullptr)
    {
        std::unique_ptr<AudioFormatReaderSource> tempSource (new AudioFormatReaderSource (tempReader, true));
        transport.setSource (tempSource.get (), 0, nullptr, tempReader->sampleRate);
        playerSource.reset (tempSource.release ());

        transport.setPosition (0.0f);

        changeState (sStop);

        LabelmusicInfo.setText (musicFile.getFileNameWithoutExtension (), juce::dontSendNotification);

        m_hotCue.reset ();

        interpretTrackInfo ();

        m_visualiser.changeSource (&musicFile);
        m_wavDisplay->changeSource (&musicFile);

        lastLoad = Time::getCurrentTime ().toMilliseconds ();

        trackInfo = theMusicDataBase->getMusicInfo (musicFile.getFullPathName ());
    }
}

//==============================================================================
void MusicPlayer::changeState (TransportState newState)
{
    state = newState;

    switch (state)
    {
    case sEmpty:
        loadButton.setEnabled (true);
        playButton.setEnabled (false);
        stopButton.setEnabled (false);
        m_hotCue.setEnabled (false);
        break;
    case sPlay:
        loadButton.setEnabled (false);
        playButton.setEnabled (true); playButton.setButtonText ("Pause");
        stopButton.setEnabled (true);
        m_hotCue.setEnabled (true);
        break;
    case sStop:
        loadButton.setEnabled (true);
        playButton.setEnabled (true); playButton.setButtonText ("Play");
        stopButton.setEnabled (transport.getCurrentPosition () == 0.0 ? false : true);
        m_hotCue.setEnabled (true);
        break;
    default:
        break;
    }
}

//===UI===========================================================================
void MusicPlayer::clickLoadButton ()
{
    load(theBrowser->getSelectedFile ());
}


void MusicPlayer::clickPlayButton ()
{
    switch (state)
    {
    case sPlay:
        transport.stop ();
        changeState (sStop);
        break;
    case sStop:
        transport.start ();
        changeState (sPlay);
        break;
    default:
        break;
    }
}

void MusicPlayer::clickStopButton ()
{    
    transport.stop ();
    transport.setPosition (0.0);

    changeState (sStop);
}

//===Timer======================================================================
void MusicPlayer::timerCallback ()
{
    if (!(state == sIdle || state == sEmpty))
    {
        LabelmusicInfo.setText (musicFile.getFileNameWithoutExtension () + " " + makeTimeString(currentPosition)
            + "/" + makeTimeString(transport.getLengthInSeconds()), juce::dontSendNotification);
    }
}

//==============================================================================
// Called when the transporter change state.
void MusicPlayer::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &transport)
    {
       if (transport.getCurrentPosition () >= transport.getLengthInSeconds ())
       {
           transport.stop ();
           transport.setPosition (0.0);
           changeState (sStop);
       }
    }

    if (source == &pitchModule)
    {
        m_wavDisplay->setTempo(2.0 - pitchModule.getCurrentSpeedFactor());
    }
}

// Create a string from a time "mm:ss:hh"
String MusicPlayer::makeTimeString (double time)
{
    String result = String ((int)(time / 60));
    result += ":";
    result += String (((int)time % 60) / 1.0);
    result += ":";
    int hunderd = (int)(time * 10) % 10;
    if (hunderd < 10)
        result += "0";
    result += String ((int)(time * 10)%10);

    return result;
}


void MusicPlayer::interpretTrackInfo ()
{
    // Iterate all hotCue's
    for (int i = 0; i < NBUTTON; ++i)
    {
        // Check if the hot cue tree is present in the track info
        if (trackInfo.indexOf (ValueTree (Identifier ("hotCue" + String (i)))) == -1)
        {
            // If not, instantly remove the hotcue
            m_hotCue.resetCue (i);
        }
        else
        {
            // Retrive hotcue info from the valuetree
            ValueTree hotCueInfo = trackInfo.getChildWithName (Identifier ("hotCue" + String (i)));
            m_hotCue.setCue (i, hotCueInfo.getProperty (musicDataIDs::hotCueCol), hotCueInfo.getProperty (musicDataIDs::hotCuePos));
        }
    }
}

void MusicPlayer::saveTrackInfo ()
{
    // Check if the transporter isn't empty
    if (state == sEmpty)
        return;

    // Iterate all hotCue's
    for (int i = 0; i < NBUTTON; ++i)
    {
        // Check if the hotcue exist
        if (m_hotCue.isCueActif (i))
        {
            ValueTree hotCueTree = trackInfo.getOrCreateChildWithName (Identifier("hotCue" + String (i)), nullptr);
            hotCueTree.setProperty (musicDataIDs::hotCuePos, m_hotCue.getCuePosition (i), nullptr);
            hotCueTree.setProperty (musicDataIDs::hotCueCol, m_hotCue.getCueColour (i), nullptr);
        }
        else
        {
            // If not, make sure their is no child for this hotcue
            trackInfo.removeChild (trackInfo.getChildWithName (Identifier ("hotCue" + String (i))), nullptr);
        }
    }
}