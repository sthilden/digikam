/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2005-06-14
 * Description : 
 * 
 * Copyright 2005 by Renchi Raju

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

// Qt includes.

#include <qimage.h>

// Local includes.

#include "dimg.h"
#include "qimageloader.h"

namespace Digikam
{

QImageLoader::QImageLoader(DImg* image)
            : DImgLoader(image)
{
}

bool QImageLoader::load(const QString& filePath, DImgLoaderObserver *observer)
{
    // Loading is opaque to us. No support for stopping from observer,
    // progress info are only pseudo values
    QImage image(filePath);

    if (observer)
        observer->progressInfo(m_image, 0.9);

    if (image.isNull())
        return false;

    m_hasAlpha = image.hasAlphaBuffer();
    
    uint w              = image.width();
    uint h              = image.height();
    unsigned char* data = new unsigned char[w*h*4];

    image.convertDepth(32);

    uint*  sptr = (uint*)image.bits();
    uchar* dptr = data;
    
    for (uint i = 0 ; i < w*h ; i++)
    {
        dptr[0] = qBlue(*sptr);
        dptr[1] = qGreen(*sptr);
        dptr[2] = qRed(*sptr);
        dptr[3] = qAlpha(*sptr);
        
        dptr += 4;
        sptr++;
    }

    if (observer)
        observer->progressInfo(m_image, 1.0);

    imageWidth()  = w;
    imageHeight() = h;
    imageData()   = data;

    // We considering that PNG is the most representative format of an image loaded by Qt
    imageSetAttribute("format", "PNG");

    return true;
}

bool QImageLoader::save(const QString& filePath, DImgLoaderObserver *observer)
{
    QVariant qualityAttr = imageGetAttribute("quality");
    int quality = qualityAttr.isValid() ? qualityAttr.toInt() : 90;
    
    if (quality < 0)
        quality = 90;
    if (quality > 100)
        quality = 100;

    QVariant formatAttr = imageGetAttribute("format");
    QCString format = formatAttr.toCString();

    QImage image = m_image->copyQImage();

    if (observer)
        observer->progressInfo(m_image, 0.1);

    // Saving is opaque to us. No support for stopping from observer,
    // progress info are only pseudo values
    bool success = image.save(filePath, format.upper(), quality);
    if (observer && success)
        observer->progressInfo(m_image, 1.0);
    return success;
}

bool QImageLoader::hasAlpha() const
{
    return m_hasAlpha;
}

}  // NameSpace Digikam
