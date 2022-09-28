/*
  ==============================================================================

    Visualiser.h
    Created: 9 May 2020 2:59:37pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Visualiser    :   public Component,
                        public ChangeListener
{
public:
    Visualiser (AudioFormatManager* p_formatManager);
    ~Visualiser ();

    //==============================================================================
    void paint (Graphics&) override;


    void changeSource (File* newSource);

private:
    AudioThumbnail m_thumbnail;
    AudioThumbnailCache m_thumbnailCache;

    bool fileLoadedFlag;

    void changeListenerCallback (ChangeBroadcaster* source) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Visualiser)
};




//==============================================================================
/* Cursor on the thumbnail to set transport position.
*/
class PositionOverlay : public Component,
                        private Timer
{
public:
    PositionOverlay (AudioTransportSource& transportSourceToUse);
    ~PositionOverlay ();

    void paint (Graphics& g) override;

    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
private:
    AudioTransportSource& transportSource;

    void timerCallback () override;
    void mouseInteraction (float position);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PositionOverlay)
};



//==============================================================================
/* thumbnail to see currentPosition
*/
class WavDisplay :  public Component,
                    public Timer
{
public:
    WavDisplay ();
    ~WavDisplay ();

    void setup (AudioFormatManager* p_formatManager, Colour playerColour,
        AudioTransportSource* transportSourceToUse);

    void paint (Graphics& g) override;

    void changeSource (File* newSource);
    void setRange (double nRange);
    void setTempo(double tempo);

    // Mouse methods for managing scrolling.
    void mouseDown (const MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseUp (const MouseEvent& event) override;


private:
    AudioThumbnail* m_thumbnail;
    AudioThumbnailCache m_thumbnailCache;

    Colour m_colour;
    AudioTransportSource* m_transport;

    float currentFrameWidthInSec;
    bool clickFlag;
    int startXPosition;
    double startTransportPosition;

    double range;
    double tempo = 1.0;
    double realRange;

    void timerCallback () override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavDisplay)
};