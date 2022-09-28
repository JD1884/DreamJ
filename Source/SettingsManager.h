/*
  ==============================================================================

    SettingsManager.h
    Created: 18 May 2020 2:04:21pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#ifdef DEBUG

#define SETUPFILE "/DreamJDatabase.xml"

#else

#define SETUPFILE "C:/ProgramData/vJD/DreamJ/DreamJDatabase.xml"

#endif // DEBUG




#define theSettingsManager SettingsManager::getInstance()


// ID's 
namespace setIDs
{
    static Identifier nameID ("DreamJ");
    static Identifier crossFaderID ("CrossFader");
    static Identifier browserID ("Browser");
    static Identifier mainID ("visualiser");
    static Identifier hotCueID ("hotCueColors");
}

//==============================================================================
/*  Singleton class to manage settings. Setings are also stored inside a XML file.
*/
class SettingsManager   //:   public Component
{
    // Singleton handlers
private:
    SettingsManager ();
    static SettingsManager* m_pInstance;
public:
    ~SettingsManager();
    static SettingsManager* getInstance ();

// Other things
    ValueTree getSpecificSettings (Identifier settupID);

private:
    ValueTree settingsTree;

    //JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsManager)
};
