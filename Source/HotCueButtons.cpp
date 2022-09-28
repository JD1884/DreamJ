/*
  ==============================================================================

    HotCueButtons.cpp
    Created: 13 May 2020 5:03:12pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HotCueButtons.h"
#include "SettingsManager.h"


//==============================================================================
HotCueButtons::HotCueButtons (AudioTransportSource& transSource, HotCueOverlay& hco) :
    transportSource (transSource),
    overlay (hco)
{
    for (int i = 0; i < NBUTTON; i++)
    {
        // Set button text
        cueButton[i].setButtonText ((String)(i + 1));

        addAndMakeVisible (cueButton[i]);
        // Disable button until track is loaded
        cueButton[i].setEnabled (false);
        // Trigger button on mouse down for better performance
        cueButton[i].setTriggeredOnMouseDown (true);

        // Add this object to the button.
        cueButton[i].addMouseListener (this, false);

        colourInUse[i] = false;
    }

    currentColour = 0;

    overlay.setMapPointers (&hotCuePosition, &buttonInUse);

    ValueTree setup = theSettingsManager->getSpecificSettings (setIDs::hotCueID);
    
    for (int i = 0; i < NBUTTON; ++i)
    {
        if (setup.hasProperty (colourID[i]))
            cueColourOrder[i] = Colour ((uint32)(int64)setup.getProperty (colourID[i]));
        else
            setup.setProperty (colourID[i], (int64) cueColourOrder[i].getARGB(), nullptr);
    }
}

HotCueButtons::~HotCueButtons()
{

}
//==============================================================================
void HotCueButtons::setEnabled (bool state)
{
    for (int i = 0; i < NBUTTON; i++)
        cueButton[i].setEnabled (state);
}

// Reset all values (when new track is loaded for instance)
void HotCueButtons::reset ()
{
    buttonInUse.clear ();
    hotCuePosition.clear ();
    for (int i = 0; i < NBUTTON; i++)
        colourInUse[i] = false;
}

//==============================================================================
void HotCueButtons::paint (Graphics& g)
{}

void HotCueButtons::resized()
{
    int buttonDimension = getWidth () / NBUTTON;
    for (int i = 0; i < NBUTTON; i++)
    {
        cueButton[i].setBounds (i * buttonDimension, 2, buttonDimension-4, buttonDimension-4);
    }
}

//==============================================================================
void HotCueButtons::mouseDown (const MouseEvent& event)
{
    // Determine witch button was clicked
    Component* source = event.originalComponent;   

    if (source == this)
        return;

    TextButton* tbSource = (TextButton*)source;

    int currentHotKey = tbSource - (TextButton*)cueButton;
    if (currentHotKey >= NBUTTON || currentHotKey < 0)
        return;

    if (buttonInUse.contains (currentHotKey))
    {
        if (event.mods.isShiftDown())
        {
            // Remove hotCue
            resetCue (currentHotKey);
        }
        else {
            // Jump to hotcue position
            transportSource.setPosition (hotCuePosition[currentHotKey]);
        }
    }
    else {
        // Create hotkey

        // Find the first available color.
        while (colourInUse[currentColour])
            currentColour = (currentColour + 1) % NBUTTON;

        setCue (currentHotKey, currentColour, transportSource.getCurrentPosition ());
    }
}

void HotCueButtons::setCue (int cueNumber, int colour, double position)
{
    buttonInUse.insert (std::pair<int, int> (cueNumber, colour));
    colourInUse[colour] = true;

    cueButton[cueNumber].setColour (TextButton::buttonColourId, cueColourOrder[colour]);

    hotCuePosition.insert (std::pair <int, double> (cueNumber, position));

    // Repaint tumbnail overlay
    overlay.repaint ();
}

void HotCueButtons::resetCue (int cueNumber)
{
    // Remove hotkey data
    colourInUse[buttonInUse[cueNumber]] = false;
    buttonInUse.erase (cueNumber);
    hotCuePosition.erase (cueNumber);

    // Make button black
    cueButton[cueNumber].setColour (TextButton::buttonColourId, Colours::black);

    // Repaint thumbnail overlay
    overlay.repaint ();
}


//==============================================================================
/*
*/

HotCueOverlay::HotCueOverlay (AudioTransportSource& p_transport):
    transportSource (p_transport)/*,
    hotCuePos (0),
    hotCueColour (0)*/
{}

HotCueOverlay::~HotCueOverlay () {}

void HotCueOverlay::paint (Graphics& g)
{
    //if (hotCuePos != 0 && hotCueColour != 0)
    //{
        for (auto i = hotCuePos->begin (); i != hotCuePos->end (); ++i)
        {
            g.setColour (cueColourOrder[hotCueColour->at (i->first)]);
            float drawPosition = (i->second / transportSource.getLengthInSeconds ()) * getWidth ();
            g.drawLine (drawPosition, 0.0f, drawPosition, getHeight (), 2.0f);
        }
    //}
}

void HotCueOverlay::setMapPointers (std::map<int, double>* hcp, std::map<int, int>* hcc)
{ 
    hotCuePos = hcp;
    hotCueColour = hcc;
}