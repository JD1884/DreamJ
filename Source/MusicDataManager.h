/*
  ==============================================================================

    MusicDataManager.h
    Created: 30 May 2020 2:00:01am
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#ifdef DEBUG

#define DATABASEFILE "/DreamJSetup.xml"

#else

#define DATABASEFILE "C:/ProgramData/vJD/DreamJ/DreamJSetup.xml"

#endif // DEBUG

#define theMusicDataBase MusicDataManager::getInstance()

//  MusicData ID's
namespace musicDataIDs
{
    static Identifier dataBaseID ("DREAMJ_LIBRARY");

    // Held the position of the hotcue in the track
    static Identifier hotCuePos ("hotCuePos");
    // Held the colour of the hotcue
    static Identifier hotCueCol ("hotCueCol");
}

// Singleton class to handle a ValueTree about the data base of the music.


//==============================================================================
/*
*/
class MusicDataManager
{
private:
    MusicDataManager();
    static MusicDataManager* m_pInstance;

public:
    ~MusicDataManager();
    static MusicDataManager* getInstance ();

    ValueTree getMusicInfo (String pathName);

private:
    ValueTree musicDataBase;
};
