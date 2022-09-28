/*
  ==============================================================================

    CrossFader.cpp
    Created: 5 May 2020 7:59:49pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CrossFader.h"

#include "SettingsManager.h"

//==============================================================================
CrossFader::CrossFader () :
    crossFader (Slider::LinearHorizontal, Slider::NoTextBox),
    nAttachedChannels(0),
    myCurve()
{
    addAndMakeVisible (&crossFader);
    crossFader.setRange (-1.0, 1.0);
    crossFader.addListener (this);

    ValueTree setupTree = theSettingsManager->getSpecificSettings (setIDs::crossFaderID);
    String initCrossStyle;
    if (setupTree.hasProperty (crossfaderSetIDs::styleID))
    {
        initCrossStyle = setupTree.getProperty (crossfaderSetIDs::styleID);
    }
    else {
        initCrossStyle = "constantgain";
        setupTree.setProperty (crossfaderSetIDs::styleID, initCrossStyle, nullptr);
    }

    if (initCrossStyle == "constantpower")
        myCurve.changeCurve (ConstantPower);
    else if (initCrossStyle == "fastcut")
        myCurve.changeCurve (FastCut);
    else if (initCrossStyle == "smooth")
        myCurve.changeCurve (Smooth);
    else if (initCrossStyle == "constantgain")
        myCurve.changeCurve (ConstantGain);

    if (setupTree.hasProperty (crossfaderSetIDs::channelsA))
        defaultAttachA = setupTree.getProperty (crossfaderSetIDs::channelsA);
    else
        defaultAttachA = 1;

    if (setupTree.hasProperty (crossfaderSetIDs::channelsB))
        defaultAttachB = setupTree.getProperty (crossfaderSetIDs::channelsB);
    else
        defaultAttachB = 2;
}

CrossFader::~CrossFader()
{
    for (auto i = buttonsSideA.begin (); i != buttonsSideA.end (); ++i)
        delete i->second;

    for (auto i = buttonsSideB.begin (); i != buttonsSideB.end (); ++i)
        delete i->second;

    attachedChannels.clear ();
    buttonsSideA.clear ();
    buttonsSideB.clear ();
    channelActifA.clear ();
    channelActifB.clear ();
}

void CrossFader::paint (Graphics& g)
{
}

void CrossFader::resized()
{
    auto A = buttonsSideA.begin (); auto B = buttonsSideB.begin ();
    switch (nAttachedChannels)
    {
    case 0:                                             //  =
        crossFader.setBounds (getLocalBounds ());
        break;

    case 1:                                             // * = *
        crossFader.setBounds (getHeight (), 0, getWidth () - 2 * getHeight (), getHeight ());
        A->second->setBounds (getHeight() / 2 - 8, getHeight () / 2 - 8, 16, 16);
        B->second->setBounds (getWidth() - getHeight () / 2 - 8, getHeight () / 2 - 8, 16, 16);
        break;

                                                        // * __ *
    case 2:                                             // *    *
        crossFader.setBounds (getHeight () / 2, 0, getWidth () - getHeight (), getHeight ());
        A->second->setBounds (getHeight () / 4 - 8, getHeight () / 4 - 4, 16, 16);
        B->second->setBounds (getWidth () - getHeight () / 4 - 8, getHeight () / 4 - 4, 16, 16);
        ++A; ++B;
        A->second->setBounds (getHeight () / 4 - 8, 3 * getHeight () / 4 - 12, 16, 16);
        B->second->setBounds (getWidth () - getHeight () / 4 - 8, 3 * getHeight () / 4 - 12, 16, 16);
        break;


                                                        //  *     *
                                                        //  * === *
    case 3:                                             //  *     *
        crossFader.setBounds (getHeight () / 3, 0, getWidth () - getHeight (), getHeight ());
        A->second->setBounds (0, 0, getHeight () / 3, getHeight () / 3);
        B->second->setBounds (getWidth () - getHeight () / 3, 0, getHeight () / 3, getHeight () / 3);
        ++A; ++B;
        A->second->setBounds (0, getHeight () / 3, getHeight () / 3, getHeight () / 3);
        B->second->setBounds (getWidth () - getHeight () / 3, getHeight () / 3, getHeight () / 3, getHeight () / 3);
        ++A; ++B;
        A->second->setBounds (0, 2 * getHeight () / 3, getHeight () / 3, getHeight () / 3);
        B->second->setBounds (getWidth () - getHeight () / 3, 2 * getHeight () / 3, getHeight () / 3, getHeight () / 3);
        break;

                                                        //  ** ___ **
    case 4:                                             //  **     **
        crossFader.setBounds (getHeight () / 2, 0, getWidth () - getHeight (), getHeight ());
        A->second->setBounds (0, 0, getHeight () / 2, getHeight () / 2);
        B->second->setBounds (getWidth () - getHeight (), 0, getHeight () / 2, getHeight () / 2);
        ++A; ++B;
        A->second->setBounds (getHeight () / 2, 0, getHeight () / 2, getHeight () / 2);
        B->second->setBounds (getWidth () - getHeight () / 2, 0, getHeight () / 2, getHeight () / 2);
        ++A; ++B;
        A->second->setBounds (0, getHeight () / 2, getHeight () / 2, getHeight () / 2);
        B->second->setBounds (getWidth () - getHeight (), getHeight () / 2, getHeight () / 2, getHeight () / 2);
        ++A; ++B;
        A->second->setBounds (getHeight () / 2, getHeight () / 2, getHeight () / 2, getHeight () / 2);
        B->second->setBounds (getWidth () - getHeight () / 2, getHeight () / 2, getHeight () / 2, getHeight () / 2);
        break;




    default:
        break;
    }
}

//==============================================================================

void CrossFader::attachChannel (MixerChannel* channel, int channelNumber)
{
    attachedChannels[channelNumber] = channel;
    buttonsSideA[channelNumber] = new TextButton (String (channelNumber));
    buttonsSideB[channelNumber] = new TextButton (String (channelNumber));

    // TODOE: make this code more sustainable
    switch (channelNumber)
    {
    case 1:
        buttonsSideA[channelNumber]->onClick = [this] { buttonClicked (false, 1); };
        buttonsSideB[channelNumber]->onClick = [this] { buttonClicked (true, 1); };
        break;
    case 2:
        buttonsSideA[channelNumber]->onClick = [this] { buttonClicked (false, 2); };
        buttonsSideB[channelNumber]->onClick = [this] { buttonClicked (true, 2); };
        break;
    case 3:
        buttonsSideA[channelNumber]->onClick = [this] { buttonClicked (false, 3); };
        buttonsSideB[channelNumber]->onClick = [this] { buttonClicked (true, 3); };
        break;
    case 4:
        buttonsSideA[channelNumber]->onClick = [this] { buttonClicked (false, 4); };
        buttonsSideB[channelNumber]->onClick = [this] { buttonClicked (true, 4); };
        break;
    case 5:
        buttonsSideA[channelNumber]->onClick = [this] { buttonClicked (false, 5); };
        buttonsSideB[channelNumber]->onClick = [this] { buttonClicked (true, 5); };
        break;
    case 6:
        buttonsSideA[channelNumber]->onClick = [this] { buttonClicked (false, 6); };
        buttonsSideB[channelNumber]->onClick = [this] { buttonClicked (true, 6); };
        break;
    default:
        break;
    }

    addAndMakeVisible (buttonsSideA[channelNumber]);
    addAndMakeVisible (buttonsSideB[channelNumber]);

   
    // Setup default attachments
    if (defaultAttachA & (1 << nAttachedChannels))
    {
        channelActifA[channelNumber] = true;
        buttonsSideA[channelNumber]->setColour (TextButton::buttonColourId, attachedChannels[channelNumber]->getColour ());
    }
    else
        channelActifA[channelNumber] = false;

    if (defaultAttachB & (1 << nAttachedChannels))
    {
        channelActifB[channelNumber] = true;
        buttonsSideB[channelNumber]->setColour (TextButton::buttonColourId, attachedChannels[channelNumber]->getColour ());
    }
    else
        channelActifB[channelNumber] = false;

    nAttachedChannels = (int)attachedChannels.size ();
}


void CrossFader::saveState ()
{
    ValueTree setupTree = theSettingsManager->getSpecificSettings (setIDs::crossFaderID);
    defaultAttachA = 0; defaultAttachB = 0;

    for (auto i = channelActifA.begin (); i != channelActifA.end (); ++i)
        defaultAttachA |= (1 << (i->first - 1)) * i->second;

    for (auto i = channelActifB.begin (); i != channelActifB.end (); ++i)
        defaultAttachB |= (1 << (i->first - 1)) * i->second;

    setupTree.setProperty (crossfaderSetIDs::channelsA, (int64)defaultAttachA, nullptr);
    setupTree.setProperty (crossfaderSetIDs::channelsB, (int64)defaultAttachB, nullptr);
}

//==============================================================================
void CrossFader::sliderValueChanged (Slider* slider)
{
    if (slider == &crossFader)
    {
        for (auto i = attachedChannels.begin (); i != attachedChannels.end (); ++i)
        {
            if (channelActifA[i->first])
                i->second->setCrossFaderValue (myCurve.getValue (false, (float) slider->getValue ()));
            if (channelActifB[i->first])
                i->second->setCrossFaderValue (myCurve.getValue (true, (float) slider->getValue ()));
        }
    }
}

void CrossFader::buttonClicked (bool side, int channelNumber)
{  
    bool* thisSide;
    bool* otherSide;
    TextButton* thisButton;
    TextButton* otherButton;

    if (side)
    {
        thisSide = &(channelActifB[channelNumber]);
        thisButton = buttonsSideB[channelNumber];
        otherSide = &(channelActifA[channelNumber]);
        otherButton = buttonsSideA[channelNumber];
    }
    else
    {
        thisSide = &(channelActifA[channelNumber]);
        thisButton = buttonsSideA[channelNumber];
        otherSide = &(channelActifB[channelNumber]);
        otherButton = buttonsSideB[channelNumber];
    }

    if (*thisSide)
    {
        (*thisSide) = false;
        attachedChannels[channelNumber]->setCrossFaderValue (1.0);
        thisButton->setColour (TextButton::buttonColourId, Colours::black);
    }
    else
    {
        (*thisSide) = true;
        (*otherSide) = false;
        thisButton->setColour (TextButton::buttonColourId, attachedChannels[channelNumber]->getColour());
        otherButton->setColour (TextButton::buttonColourId, Colours::black);
        
        sliderValueChanged (&crossFader);
    }
}


