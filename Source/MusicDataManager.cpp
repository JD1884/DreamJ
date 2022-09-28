/*
  ==============================================================================

    MusicDataManager.cpp
    Created: 30 May 2020 2:00:01am
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MusicDataManager.h"

//==============================================================================
MusicDataManager::MusicDataManager():
    musicDataBase (musicDataIDs::dataBaseID)
{

}

MusicDataManager* MusicDataManager::m_pInstance = 0;

MusicDataManager::~MusicDataManager()
{
}

MusicDataManager* MusicDataManager::getInstance ()
{
    if (m_pInstance == 0)
        m_pInstance = new MusicDataManager ();

    return m_pInstance;
}


// Returns the Value tree of the requested track
ValueTree MusicDataManager::getMusicInfo (String pathName)
{
    return musicDataBase.getOrCreateChildWithName (Identifier (pathName), nullptr);
}