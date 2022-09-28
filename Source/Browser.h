/*
  ==============================================================================

    Browser.h
    Created: 6 May 2020 11:33:27pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "SettingsManager.h"

namespace browserSetIDs
{
    static Identifier defaultLocation ("defaultLocation");
}

//==============================================================================
/*
*/

#define theBrowser Browser::getInstance()

class Browser    : public Component,
                   public FileBrowserListener,
                   public ActionBroadcaster
{
private:
    Browser();   
    static Browser* m_instance;

public:
    ~Browser ();
    static Browser* getInstance ();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    File getSelectedFile ();

    //==============================================================================
    virtual void selectionChanged () {}
    virtual void fileClicked (const File& file, const MouseEvent& e) {}
    virtual void fileDoubleClicked (const File& file);
    virtual void browserRootChanged (const File& newRoot) {}

private:
    FileBrowserComponent m_fbrowserc;
    WildcardFileFilter m_fileFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Browser)
};
