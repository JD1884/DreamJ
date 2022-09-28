/*
  ==============================================================================

    Visualiser.cpp
    Created: 9 May 2020 2:59:37pm
    Author:  jensd

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Visualiser.h"

//==============================================================================
Visualiser::Visualiser (AudioFormatManager* p_formatManager) :
    m_thumbnailCache (5),
    m_thumbnail (16384, *p_formatManager, m_thumbnailCache)
{
    fileLoadedFlag = false;
    m_thumbnail.addChangeListener (this);
}

Visualiser::~Visualiser()
{
   if (fileLoadedFlag)
        m_thumbnail.setSource (nullptr);
}

void Visualiser::paint (Graphics& g)
{
    g.setColour (Colours::white);
    g.fillRect (getLocalBounds ());

    if (fileLoadedFlag)
    {
        g.setColour (Colours::black);
        m_thumbnail.drawChannel (g, getLocalBounds (), 0.0, m_thumbnail.getTotalLength (), 0, 0.95f);
    }
}

void Visualiser::changeSource (File* newSource)
{
    m_thumbnail.setSource (new FileInputSource (*newSource));
    fileLoadedFlag = true;
}

void Visualiser::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &m_thumbnail)
        repaint ();
}

//==============================================================================
PositionOverlay::PositionOverlay (AudioTransportSource& transportSourceToUSe) :
    transportSource (transportSourceToUSe)
{
    startTimer (40);
}

PositionOverlay::~PositionOverlay () {}


void PositionOverlay::paint (Graphics& g)
{
    float duration = transportSource.getLengthInSeconds ();

    if (duration > 0.0)
    {
        float drawPosition = (transportSource.getCurrentPosition () / duration) * getWidth ();

        g.setColour (Colours::white);
        g.drawLine (drawPosition, 0.0f, drawPosition, getHeight (), 2.0f);
    }
}

void PositionOverlay::mouseDown (const MouseEvent& event)
{
    mouseInteraction (event.position.x);
}

void PositionOverlay::mouseDrag (const MouseEvent& event)
{
    mouseInteraction (event.position.x);
}

void PositionOverlay::timerCallback ()
{
    repaint ();
}

void PositionOverlay::mouseInteraction (float position)
{
    auto duration = transportSource.getLengthInSeconds ();

    if (duration > 0.0f)
    {
        transportSource.setPosition ((position / getWidth ()) * duration);
    }
}


//==============================================================================
WavDisplay::WavDisplay () :
    m_thumbnailCache (2),
    m_thumbnail (NULL),
    m_colour (Colours::white),
    m_transport (NULL),
    currentFrameWidthInSec (4.0f),
    clickFlag (false),
    range (2.0f),
    realRange(2.0f)
{}
WavDisplay::~WavDisplay ()
{
    if (m_transport->getCurrentPosition () > 0.0)
        m_thumbnail->setSource (nullptr);

    if (m_thumbnail != NULL)
        delete m_thumbnail;
}

void WavDisplay::setup (AudioFormatManager* p_formatManager, Colour playerColour,
    AudioTransportSource* transportSourceToUse)
{
    m_colour = playerColour;
    m_thumbnail = new AudioThumbnail (2, *p_formatManager, m_thumbnailCache);
    m_transport = transportSourceToUse;

    startTimer (10);
}

void WavDisplay::paint (Graphics& g)
{
    if (m_thumbnail != NULL)
    {
        float duration = m_transport->getLengthInSeconds ();

        // Check if something is loaded in the transporter
        if (duration > 0.0)
        {
            double currentPosition = m_transport->getCurrentPosition ();

            g.setColour (m_colour);
            m_thumbnail->drawChannel (g, getLocalBounds (), currentPosition - realRange, currentPosition + realRange, 0, 1.0f);
        }
    }
}

void WavDisplay::changeSource (File* newSource)
{
    m_thumbnail->setSource (new FileInputSource (*newSource));
}

void WavDisplay::setRange(double nRange)
{
    range = nRange;
    realRange = tempo * range;
    currentFrameWidthInSec = 2 * realRange;
}

void WavDisplay::setTempo(double tempo)
{
    this->tempo = tempo;
    setRange(range);
}

void WavDisplay::mouseDown (const MouseEvent& event)
{
    if (m_transport->isPlaying ())
    {
        clickFlag = true;
    }

    m_transport->stop ();
    startXPosition = event.position.x;
    startTransportPosition = m_transport->getCurrentPosition ();
}
void WavDisplay::mouseDrag (const MouseEvent& event)
{
    double newPosition = startTransportPosition -
        ((double)(event.position.x - (double)startXPosition) / (double)getWidth ()) * currentFrameWidthInSec;

    if (newPosition < 0.0)
        newPosition = 0.0;
    else
    if (newPosition > m_transport->getTotalLength ())
        newPosition = m_transport->getTotalLength ();
    
    m_transport->setPosition(newPosition);
}
void WavDisplay::mouseUp (const MouseEvent& event)
{
    if (clickFlag)
    {
        m_transport->start ();
        clickFlag = false;
    }
}

void WavDisplay::timerCallback ()
{
    repaint ();
}