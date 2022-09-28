/*
  ==============================================================================

    SettingsManager.cpp
    Created: 18 May 2020 2:04:21pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SettingsManager.h"


//==============================================================================
SettingsManager::SettingsManager() :
    settingsTree (setIDs::nameID)
{
#ifdef DEBUG
    File xmlFile (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory ().getFullPathName () + SETUPFILE);
#else
    File xmlFile (SETUPFILE);
#endif

    if (xmlFile.exists ())
    {
        settingsTree = ValueTree::fromXml(*parseXML (xmlFile));
    }
}

SettingsManager* SettingsManager::m_pInstance = 0;

SettingsManager::~SettingsManager()
{
    std::unique_ptr<XmlElement> XMLVersion = settingsTree.createXml ();

#ifdef DEBUG
    File xmlFile (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory ().getFullPathName () + SETUPFILE);
#else
    File xmlFile (SETUPFILE);
#endif

    xmlFile.create ();

    XMLVersion->writeTo (xmlFile);
}

SettingsManager* SettingsManager::getInstance ()
{
    if (m_pInstance == 0)
        m_pInstance = new SettingsManager;

    return m_pInstance;
}


//==============================================================================
ValueTree SettingsManager::getSpecificSettings (Identifier settupId)
{
    return settingsTree.getOrCreateChildWithName (settupId, nullptr);
}