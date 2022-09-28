/*
  ==============================================================================

    Browser.cpp
    Created: 6 May 2020 11:33:27pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Browser.h"



//==============================================================================
Browser::Browser () :
    m_fileFilter("*.mp3; *.wav; *.flac", "", "Browser"),
    m_fbrowserc (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
        File::getSpecialLocation(File::userMusicDirectory),
        &m_fileFilter,
        NULL)
{
    addAndMakeVisible (&m_fbrowserc);
    m_fbrowserc.addListener (this);

    ValueTree setupTree = theSettingsManager->getSpecificSettings (setIDs::browserID);
    if (setupTree.hasProperty (browserSetIDs::defaultLocation))
    {
        String defaultBrowseLocation = setupTree.getProperty (browserSetIDs::defaultLocation);
        m_fbrowserc.setRoot (File (defaultBrowseLocation));
    }
}

Browser::~Browser()
{
    // Save default browser location for next use.
    ValueTree setupTree = theSettingsManager->getSpecificSettings (setIDs::browserID);
    setupTree.setProperty (browserSetIDs::defaultLocation, m_fbrowserc.getRoot ().getFullPathName (), nullptr);
}

Browser* Browser::m_instance = 0;

Browser* Browser::getInstance ()
{
    if (m_instance == 0)
        m_instance = new Browser ();

    return m_instance;
}

//==============================================================================
void Browser::paint (Graphics& g)
{
}

void Browser::resized()
{
    m_fbrowserc.setBounds (getLocalBounds ());
}


//==============================================================================
File Browser::getSelectedFile ()
{
    return m_fbrowserc.getHighlightedFile ();
}

//==============================================================================
void Browser::fileDoubleClicked (const File& file)
{
    sendActionMessage (file.getFullPathName ());
}