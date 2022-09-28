/*
  ==============================================================================

    HotCueButtons.h
    Created: 13 May 2020 5:03:12pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define NBUTTON 8

// This is the order from how color's are allocated to the hotcue's.
static Colour cueColourOrder[NBUTTON] =
{
    Colour (0xff, 0x00, 0x80),
    Colour (0x7b, 0xff, 0x00),
    Colour (0x00, 0xfb, 0xff),
    Colour (0xff, 0x40, 0x00),
    Colour (0x09, 0x00, 0xff),
    Colour (0xfb, 0xff, 0x00),
    Colour (0xff, 0x80, 0x00),
    Colour (0xff, 0x00, 0xfb)
};

const Identifier colourID[NBUTTON] =
{
    ("Col1"),
    ("Col2"),
    ("Col3"),
    ("Col4"),
    ("Col5"),
    ("Col6"),
    ("Col7"),
    ("Col8")
};

//==============================================================================
/*
*/
class HotCueOverlay : public Component
{
public:
    //==============================================================================
    HotCueOverlay (AudioTransportSource& p_transport);
    ~HotCueOverlay ();

    void setMapPointers (std::map<int, double>* hcp, std::map<int, int>* hcc);

    //==============================================================================
    void paint (Graphics&) override;

private:
    AudioTransportSource& transportSource;

    std::map<int, double>* hotCuePos;
    std::map<int, int>* hotCueColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HotCueOverlay)
};


//==============================================================================
/*
*/
class HotCueButtons    :    public Component
{
public:
    //==============================================================================
    HotCueButtons(AudioTransportSource& transSource, HotCueOverlay& hco);
    ~HotCueButtons();

    //==============================================================================
    void setEnabled (bool state);
    void reset ();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseDown (const MouseEvent& event) override;

    //==============================================================================
    void setCue (int cueNumber, int colour, double position);
    void resetCue (int cueNumber);

    bool isCueActif (int cueNumber) { return buttonInUse.contains (cueNumber); }
    double getCuePosition (int cueNumber) { return isCueActif (cueNumber) ? hotCuePosition[cueNumber] : 0; }
    int getCueColour (int cueNumber) { return buttonInUse [cueNumber]; }

private:
    TextButton cueButton[NBUTTON];

    AudioTransportSource& transportSource;

    // When a button is linked to a button, then it is linked to a colour in this map.
    std::map <int, int> buttonInUse;
    bool colourInUse[NBUTTON];
    int currentColour;
    std::map<int, double> hotCuePosition;

    HotCueOverlay& overlay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HotCueButtons)
};


