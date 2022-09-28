/*
  ==============================================================================

    CrossFader.h
    Created: 5 May 2020 7:59:49pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "MixerChannel.h"
#include "CrossfaderStyles.h"

// crossfader setup ID's
namespace crossfaderSetIDs
{
    // Identifier to save crossFader style; default constant Gain
    static Identifier styleID ("style");
    // Identifier for attached channels to the A and B side. A bit define a channel
    static Identifier channelsA ("attachToA");
    static Identifier channelsB ("attachToB");
}

//==============================================================================
/*
*/
class CrossFader    :   public Component,
                        public Slider::Listener
                        
{
public:
    //==============================================================================
    CrossFader();
    ~CrossFader();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    

    //==============================================================================
    void attachChannel (MixerChannel* channel, int channelNumber);

    void saveState ();

private:


    Slider crossFader;

    std::map<int, MixerChannel*> attachedChannels;
    std::map<int, TextButton*> buttonsSideA;
    std::map<int, TextButton*> buttonsSideB;

    std::map<int, bool> channelActifA;
    std::map<int, bool> channelActifB;

    int nAttachedChannels;
    int defaultAttachA, defaultAttachB;

    CrossFaderCurve myCurve;

    //==============================================================================
    void sliderValueChanged (Slider* slider) override;
    void buttonClicked (bool side, int channelNumber);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrossFader)
};
