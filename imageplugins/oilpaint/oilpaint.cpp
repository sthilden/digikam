/* ============================================================
 * File  : oilpaint.cpp
 * Author: Gilles Caulier <caulier dot gilles at kdemail dot net>
           Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Date  : 2005-05-25
 * Description : OilPainting threaded image filter.
 *
 * Copyright 2005 by Gilles Caulier
 * Copyright 2006 by Gilles Caulier and Marcel Wiesweg
 *
 * Original OilPaint algorithm copyrighted 2004 by 
 * Pieter Z. Voloshyn <pieter_voloshyn at ame.com.br>.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */
 
// C++ includes. 
 
#include <cmath>
#include <cstdlib>

// Local includes.

#include "oilpaint.h"

namespace DigikamOilPaintImagesPlugin
{

OilPaint::OilPaint(Digikam::DImg *orgImage, QObject *parent, int brushSize, int smoothness)
        : Digikam::DImgThreadedFilter(orgImage, parent, "OilPaint")
{
    m_brushSize  = brushSize;
    m_smoothness = smoothness;
    initFilter();
}

void OilPaint::filterImage(void)
{
    oilpaintImage(m_orgImage, m_destImage, m_brushSize, m_smoothness);
}

// This method have been ported from Pieter Z. Voloshyn algorithm code.

/* Function to apply the OilPaint effect.                
 *                                                                                    
 * data             => The image data in RGBA mode.                            
 * w                => Width of image.                          
 * h                => Height of image.                          
 * BrushSize        => Brush size.
 * Smoothness       => Smooth value.                                                
 *                                                                                  
 * Theory           => Using MostFrequentColor function we take the main color in  
 *                     a matrix and simply write at the original position.            
 */                                                                                 
    
void OilPaint::oilpaintImage(Digikam::DImg &orgImage, Digikam::DImg &destImage, int BrushSize, int Smoothness)
{
    int    progress;
    Digikam::DColor mostFrequentColor;
    int    w,h;

    mostFrequentColor.setSixteenBit(orgImage.sixteenBit());
    w = (int)orgImage.width();
    h = (int)orgImage.height();

    for (int h2 = 0; !m_cancel && (h2 < h); h2++)
    {
        for (int w2 = 0; !m_cancel && (w2 < w); w2++)
        {
            mostFrequentColor = MostFrequentColor(orgImage, w2, h2, BrushSize, Smoothness);
            destImage.setPixelColor(w2, h2, mostFrequentColor);
        }

        progress = (int) (((double)h2 * 100.0) / h);
        if ( progress%5 == 0 )
            postProgress( progress );
    }
}

// This method have been ported from Pieter Z. Voloshyn algorithm code.

/* Function to determine the most frequent color in a matrix                        
 *                                                                                
 * Bits             => Bits array                                                    
 * Width            => Image width                                                   
 * Height           => Image height                                                 
 * X                => Position horizontal                                           
 * Y                => Position vertical                                            
 * Radius           => Is the radius of the matrix to be analized                  
 * Intensity        => Intensity to calcule                                         
 *                                                                                  
 * Theory           => This function creates a matrix with the analized pixel in   
 *                     the center of this matrix and find the most frequenty color   
 */

Digikam::DColor OilPaint::MostFrequentColor(Digikam::DImg &src, int X, int Y, int Radius, int Intensity)
{
    int  i, w, h, I, Width, Height;
    uint red, green, blue;
    Digikam::DColor mostFrequentColor;

    double Scale = Intensity / (src.sixteenBit() ? 65535.0 : 255.0);
    Width = (int)src.width();
    Height = (int)src.height();

    // Alloc some arrays to be used
    uchar *IntensityCount = new uchar[(Intensity + 1) * sizeof (uchar)];
    uint  *AverageColorR  = new uint[(Intensity + 1)  * sizeof (uint)];
    uint  *AverageColorG  = new uint[(Intensity + 1)  * sizeof (uint)];
    uint  *AverageColorB  = new uint[(Intensity + 1)  * sizeof (uint)];

    // Erase the array
    memset(IntensityCount, 0, (Intensity + 1) * sizeof (uchar));

    for (w = X - Radius; w <= X + Radius; w++)
    {
        for (h = Y - Radius; h <= Y + Radius; h++)
        {
            // This condition helps to identify when a point doesn't exist

            if ((w >= 0) && (w < Width) && (h >= 0) && (h < Height))
            {
                Digikam::DColor color = src.getPixelColor(w, h);
                red           = (uint)color.red();
                green         = (uint)color.green();
                blue          = (uint)color.blue();

                I = (uint)(GetIntensity (red, green, blue) * Scale);
                IntensityCount[I]++;

                if (IntensityCount[I] == 1)
                {
                    AverageColorR[I] = red;
                    AverageColorG[I] = green;
                    AverageColorB[I] = blue;
                }
                else
                {
                    AverageColorR[I] += red;
                    AverageColorG[I] += green;
                    AverageColorB[I] += blue;
                }
            }
        }
    }

    I = 0;
    int MaxInstance = 0;

    for (i = 0 ; i <= Intensity ; i++)
    {
        if (IntensityCount[i] > MaxInstance)
        {
            I = i;
            MaxInstance = IntensityCount[i];
        }
    }

    // get Alpha channel value from original (unchanged)
    mostFrequentColor = src.getPixelColor(X, Y);

    // Overwrite RGB values to destination.
    mostFrequentColor.setRed(AverageColorR[I] / MaxInstance);
    mostFrequentColor.setGreen(AverageColorG[I] / MaxInstance);
    mostFrequentColor.setBlue(AverageColorB[I] / MaxInstance);

    // free all the arrays
    delete [] IntensityCount;
    delete [] AverageColorR;
    delete [] AverageColorG;
    delete [] AverageColorB;

    return mostFrequentColor;
}

}  // NameSpace DigikamOilPaintImagesPlugin
