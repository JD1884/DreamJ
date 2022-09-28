/*
  ==============================================================================

    MusicPlayer.h
    Created: 5 May 2020 1:29:28pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "vJDSoundSource.h"
#include "Browser.h"
#include "Visualiser.h";
#include "HotCueButtons.h"
#include "MusicDataManager.h"
#include "PitchModule.h"


//==============================================================================
/*
*/
class MusicPlayer    :  public vJDSoundSource,
                        public Timer,
                        public ChangeListener
{
public:
    //===Constructor/Destructor=====================================================
    MusicPlayer (String tName, Colour tColour, AudioFormatManager* p_formatManager);
    ~MusicPlayer();

    //===AudioSource================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources () override;

    //===Component==================================================================
    void paint (Graphics&) override;
    void resized () override;

    void attachWavDisplay (WavDisplay* theWavDisplay);

    //==============================================================================
    void load (File mFile);

    //==============================================================================
    Colour getColour () override { return playerColour; }
    bool isPlaying () { return transport.isPlaying (); }
    int64 getLastLoad () { return lastLoad; }

    // Call this after loading a new track
    void interpretTrackInfo ();
    // Call this before removing a track from this player.
    void saveTrackInfo ();

private:
    //==============================================================================
    String playerName;
    Colour playerColour;

    //==============================================================================
    typedef enum
    {
        sIdle,      // State when starting up
        sEmpty,     // State when their is no audio file loaded
        sPlay,      // State when audio is playing.
        sStop       // State when audio is paused
    } TransportState;

    TransportState state;

    void changeState (TransportState newState);

    //===UI===========================================================================
    Label LabelmusicInfo;

    TextButton loadButton;
    TextButton playButton;
    TextButton stopButton;

    HotCueButtons m_hotCue;
    HotCueOverlay m_hotCueOverlay;

    void clickLoadButton ();
    void clickPlayButton ();
    void clickStopButton ();

    //===Audio fields================================================================
    AudioFormatManager* formatManager;
    // Place where audio is played from.
    std::unique_ptr<AudioFormatReaderSource> playerSource;
    AudioTransportSource transport;
    File musicFile;
    // ResamplingAudioSource tempoManager;

    // Time stamp of the last successfull loading, in milliseconds
    int64 lastLoad;
    ValueTree trackInfo;

    // Field to hold the current position of the transporter. Updated every getNextAudioBlock
    double currentPosition;

    PitchModule pitchModule;

    //===Timer======================================================================
    void timerCallback () override;

    //===Visualiser=================================================================
    Visualiser m_visualiser;
    PositionOverlay visCursor;
    WavDisplay* m_wavDisplay;

    //==============================================================================
    void changeListenerCallback (ChangeBroadcaster* source) override;

    String makeTimeString (double time);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MusicPlayer)
};
