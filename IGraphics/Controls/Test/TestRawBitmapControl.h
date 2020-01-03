/*
 ==============================================================================

 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.

 See LICENSE.txt for  more info.

 ==============================================================================
*/

#pragma once

/**
 * @file
 * @copydoc TestRawBitmapControl
 */

#include "IControl.h"

/** Control to test obtaining a drawing API (NanoVG, LICE, Cairo, AGG etc) context and using that API within an IControl
 *   @ingroup TestControls */
class TestRawBitmapControl : public IControl
{
public:
  TestRawBitmapControl(const IRECT& bounds)
  : IControl(bounds)
  {
    SetTooltip("TestRawBitmapControl");
  }

  void OnRescale() override
  {
    int scale = GetUI()->GetScreenScale();
    IRECT rect = mRECT.GetPadded(-10.f);
    GetUI()->CreateRawBitmap(mBitmap, rect.W() * scale, rect.H() * scale);
      
    for (int i = 0; i < mBitmap.W(); i++)
    {
      for (int j = 0; j < mBitmap.H(); j++)
      {
        unsigned char d = 255 * (float) (mBitmap.H() - j) / mBitmap.H();
        unsigned char c = 255 - ((((i / 8) % 2) ^ ((j / 8) % 2)) * d);
        
        mBitmap.SetPixel(i, j, IColor(255, c, c, c));
      }
    }
  }

  void OnInit() override
  {
    OnRescale();
  }

  void Draw(IGraphics& g) override
  {
    IRawBitmap transientBitmap;
    IRECT rect1 = mRECT.GetPadded(-10.f);
    IRECT rect2 = rect1.GetScaled(g.GetScreenScale());

    g.FillRect(COLOR_BLACK, mRECT);
    g.DrawRawBitmap(mBitmap, rect1);
      
    g.CreateRawBitmap(transientBitmap, rect2.W() * 0.3, rect2.H() * 0.3);
      
    for (int i = 0; i < transientBitmap.W(); i++)
    {
      for (int j = 0; j < transientBitmap.H(); j++)
        transientBitmap.SetPixel(i, j, IColor(50, 255, 0, 0));
    }
      
    g.DrawRawBitmap(transientBitmap, rect1);
  }

private:
  IRawBitmap mBitmap;
};
