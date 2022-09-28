/*
  ==============================================================================

    PitchModule.cpp
    Created: 31 Aug 2022 8:48:32pm
    Author:  jensd

  ==============================================================================
*/

#include "PitchModule.h"

#define MAXSPEEDFACTOR 100
#define RATIOUPDATETIME 100

//==============================================================================
PitchModule::PitchModule(AudioSource* myAudioSource) :
    pitchSlider(Slider::LinearVertical, Slider::NoTextBox),
    speedSlider(Slider::LinearVertical, Slider::NoTextBox),
    resetSlidersButton("0"),
    keyLockButton("Key lock"),
    rangeTextEditor(),
    myAudioSource(myAudioSource)
{
    addAndMakeVisible(pitchSlider);
    pitchSlider.setRange(-1.0, 1.0);
    pitchSlider.setValue(0.0);
    pitchSlider.addListener(this);

    addAndMakeVisible(speedSlider);
    speedSlider.setRange(-1.0, 1.0);
    speedSlider.setValue(0.0);
    speedSlider.addListener(this);

    addAndMakeVisible(resetSlidersButton);
    resetSlidersButton.onClick = [this] { resetSliders(); };

    addAndMakeVisible(keyLockButton);
    keyLockButton.addListener(this);
    keyLockButton.setToggleState(true, NotificationType::sendNotification);

    addAndMakeVisible(rangeTextEditor);
    rangeTextEditor.addListener(this);
    rangeTextEditor.setText("50%");

    myRubberBandRatioTimer.owner = this;
    myRubberBandRatioTimer.startTimer(RATIOUPDATETIME);
} 

PitchModule::~PitchModule()
{
    delete rubberBand;
    delete sourceBuffer;
}

void PitchModule::paint (juce::Graphics& g)
{

}

void PitchModule::resized()
{
    int sliderHeight = getHeight() - 60;

    pitchSlider.setBounds(0.5 * getWidth(), 0, 0.3 * getWidth() / 2, sliderHeight);
    speedSlider.setBounds(0.1 * getWidth(), 0, 0.3 * getWidth() / 2, sliderHeight);
    resetSlidersButton.setBounds(0.8 * getWidth(), sliderHeight / 2 - 0.075 * getWidth(), 0.15 * getWidth(), 0.15 * getWidth());

    rangeTextEditor.setBounds(0.2 * getWidth(), getHeight() - 54, 0.6 * getWidth(), 24);
    keyLockButton.setBounds(0.3 * getWidth(), getHeight() - 27, 0.4 * getWidth(), 24);
}

//===AudioSource==================================================================
void PitchModule::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    buffersLength = samplesPerBlockExpected;

    sourceBuffer = new AudioBuffer<float>(2, buffersLength);

    rubberBand = new RubberBand::RubberBandStretcher(sampleRate, 2,
        RubberBand::RubberBandStretcher::Option::OptionProcessRealTime  |       // OptionProcessOffline
        RubberBand::RubberBandStretcher::Option::OptionTransientsSmooth |       // OptionTransientsCrisp, OptionTransientsMixed
        RubberBand::RubberBandStretcher::Option::OptionDetectorCompound |       // OptionDetectorPercussive, OptionDetectorSoft
        RubberBand::RubberBandStretcher::Option::OptionPhaseLaminar     |       // OptionPhaseIndependent
        RubberBand::RubberBandStretcher::Option::OptionThreadingAuto    |       // OptionThreadingNever, OptionThreadingAlways
        RubberBand::RubberBandStretcher::Option::OptionWindowStandard   |       // OptionWindowShort, OptionWindowLong
        RubberBand::RubberBandStretcher::Option::OptionSmoothingOn      |       // OptionSmoothingOff
        RubberBand::RubberBandStretcher::Option::OptionFormantShifted   |       // OptionFormantPreserved
        RubberBand::RubberBandStretcher::Option::OptionPitchHighSpeed   |       // OptionPitchHighQuality, OptionPitchHighConsistency
        RubberBand::RubberBandStretcher::Option::OptionChannelsApart    |       // OptionChannelsTogether
        RubberBand::RubberBandStretcher::Option::OptionEngineFaster);           //  OptionEngineFiner

    rubberBand->reset();
}

void PitchModule::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    AudioSourceChannelInfo sourceChannelInfo(sourceBuffer, 0, buffersLength);
    float** arrayOfWritePointers = bufferToFill.buffer->getArrayOfWritePointers();
    const float** arrayOfReadPointers = sourceBuffer->getArrayOfReadPointers();

    int nSamplesAvailable = rubberBand->available();

    while (nSamplesAvailable < buffersLength)
    {
        myAudioSource->getNextAudioBlock(sourceChannelInfo);

        rubberBand->process(arrayOfReadPointers, buffersLength, false);

        nSamplesAvailable = rubberBand->available();
    }

    rubberBand->retrieve(bufferToFill.buffer->getArrayOfWritePointers(), buffersLength);
}

void PitchModule::releaseResources()
{
    rubberBand->reset();
    sourceBuffer->clear();
}


//==============================================================================
void PitchModule::sliderValueChanged(Slider* slider)
{
    calcTempoFactor();
    calcPitchFactor();
    if (isInKeyLockMode)
    {
        if (slider == &speedSlider)
        {
            pitchSlider.setValue(
                (1 - 1 / (keyLockPitchValue * currentSpeedFactor)) /
                slidersRange
            );
        }
    }
}

void PitchModule::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if (&textEditor == &rangeTextEditor)
    {
        String inputText = rangeTextEditor.getText();
        if (inputText.contains("%"))
            inputText.replace("%", "");

        String refString = ".";
        for (int i = 0; i < 10; i++)
            refString += '0' + i;

        if (inputText.containsOnly(refString))
        {
            float inputFloat = inputText.getFloatValue();

            if (inputFloat >= 5 && inputFloat <= 100)
                slidersRange = inputFloat / 100.0;
            else if (inputFloat >= 0.05 && inputFloat <= 1.0)
                slidersRange = inputFloat;
        }

        rangeTextEditor.setText(String::String(slidersRange * 100.0) + "%");

        speedSlider.setValue((1 - 1 / currentSpeedFactor) / slidersRange);
        pitchSlider.setValue((1 - 1 / (currentSpeedFactor * currentPitchFactor)) / slidersRange);
    }
}

void PitchModule::buttonClicked(Button* button)
{
    if (button == &keyLockButton)
    {
        bool newValue = keyLockButton.getToggleState();
        if (newValue != isInKeyLockMode)
        {
            isInKeyLockMode = newValue;

            if (isInKeyLockMode)
            {
                keyLockPitchValue = currentPitchFactor;
                pitchSlider.setEnabled(false);
            }
            else
                pitchSlider.setEnabled(true);
        }
    }
}


void PitchModule::setTimeStretchRatios()
{
    rubberBand->setPitchScale(currentPitchFactor);
    rubberBand->setTimeRatio(currentSpeedFactor);
}

float PitchModule::getCurrentSpeedFactor()
{
    return currentSpeedFactor;
}


void PitchModule::rubberBandRatioTimer::timerCallback()
{
    if (factorChanged)
    {
        factorChanged = false;

        owner->setTimeStretchRatios();
    }
}

void PitchModule::resetSliders()
{
    pitchSlider.setValue(0.0);
    speedSlider.setValue(0.0);
}

void PitchModule::calcTempoFactor()
{
    float speedSliderPos = speedSlider.getValue();
    if (slidersRange * speedSliderPos >= 1)
        currentSpeedFactor = MAXSPEEDFACTOR;  // = inf.
    else
        currentSpeedFactor = 1 / (1 - slidersRange * speedSliderPos);

    if (currentSpeedFactor > MAXSPEEDFACTOR)
        currentSpeedFactor = MAXSPEEDFACTOR;

    myRubberBandRatioTimer.factorChanged = true;

    sendChangeMessage();
}

void PitchModule::calcPitchFactor()
{
    if (currentSpeedFactor == MAXSPEEDFACTOR)
        currentPitchFactor = 0.1;
    else
    {
        float pitchSliderPos = pitchSlider.getValue();
        if (slidersRange * pitchSliderPos >= 1)
            currentPitchFactor = MAXSPEEDFACTOR;
        else
            currentPitchFactor = 1 / (currentSpeedFactor * (1 - slidersRange * pitchSliderPos));
    }
}