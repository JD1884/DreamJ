/*
  ==============================================================================

    FaderStyles.h
    Created: 6 May 2020 3:54:52pm
    Author:  jensd

  ==============================================================================
*/

#pragma once

// Style source: https://djplayerapp.wordpress.com/2013/04/03/crossfader/

enum CrossFaderStyle
{
    ConstantPower,
    FastCut,
    Smooth,
    ConstantGain
};


float CrossFaderStyle_None_getValue (bool side, float sliderVal);

float CrossFaderStyle_ConstantPower_getValue (bool side, float sliderVal);
float CrossFaderStyle_FastCut_getValue (bool side, float sliderVal);
float CrossFaderStyle_Smooth_getValue (bool side, float sliderVal);
float CrossFaderStyle_ConstantGain_getValue (bool side, float sliderVal);

class CrossFaderCurve
{
public:
    CrossFaderCurve () { getValue = CrossFaderStyle_None_getValue; };
    CrossFaderCurve (CrossFaderStyle style) { changeCurve (style); };
    ~CrossFaderCurve () { getValue = nullptr; };

    void changeCurve (CrossFaderStyle style);
    float (*getValue) (bool side, float sliderVal);
};