/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "MusicPlayer.h"
#include "MixerModule.h"
#include "Browser.h"
#include "SettingsManager.h"

namespace mainIDs
{
    static Identifier wavDisplayZoomerVal ("wavZoomer");
}

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent,
                        public Slider::Listener,
                        public ActionListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void sliderValueChanged (Slider* slider) override;
    // Called by the browser when dubbel clicked on a file.
    virtual void actionListenerCallback (const String& message) override;

private:

    void saveState ();

    AudioFormatManager formatManager;
    MusicPlayer musicPlayerA;
    WavDisplay musicPlayerAVisualiser;
    MusicPlayer musicPlayerB;
    WavDisplay musicPlayerBVisualiser;
    MixerModule mixerModule;   // This is a pointer for the reason that it can be destroyed before the settingsmanager.

    Slider wavDisplayZoomer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
