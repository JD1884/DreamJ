/*
  ==============================================================================

    FaderStyles.cpp
    Created: 6 May 2020 3:54:52pm
    Author:  jensd

  ==============================================================================
*/

#include "CrossFaderStyles.h"

//==============================================================================

void CrossFaderCurve::changeCurve (CrossFaderStyle style)
{
    switch (style)
    {
    case ConstantPower:
        getValue = CrossFaderStyle_ConstantPower_getValue;
        break;
    case FastCut:
        getValue = CrossFaderStyle_FastCut_getValue;
        break;
    case Smooth:
        getValue = CrossFaderStyle_Smooth_getValue;
        break;
    case ConstantGain:
        getValue = CrossFaderStyle_ConstantGain_getValue;
        break;

    default:
        getValue = CrossFaderStyle_ConstantGain_getValue;
        break;
    }
}


//##############################################################################
float CrossFaderStyle_None_getValue (bool side, float sliderVal)
{
    return 1.0;
}

/*
=======

*/


//##############################################################################
/*
ConstantPower
--- ----
   X
  / \
 /   \
|     |

*/
float CrossFaderStyle_ConstantPower_getValue (bool side, float sliderVal)
{
    // TODO
    return 1.0;
}

//##############################################################################
/*
FastCut
--======--
 /      \
/        \

*/
float CrossFaderStyle_FastCut_getValue (bool side, float sliderVal)
{
    if (side)
        sliderVal *= -1.0;

    if (sliderVal > 0.8f)
        return -5.0f * sliderVal + 5.0f;

    return 1.0f;
}


//##############################################################################
/*
Smooth
--   --
  \ /
   X
  / \
 /   \
|     |

*/
float CrossFaderStyle_Smooth_getValue (bool side, float sliderVal)
{
    return 1.0;
}



//##############################################################################
/*
ConstantGain

\   /
 \ /
  X
 / \
/   \

*/
float CrossFaderStyle_ConstantGain_getValue (bool side, float sliderVal)
{
    if (!side)
        sliderVal *= -1.0;

    return (sliderVal + 1) / 2;
}

