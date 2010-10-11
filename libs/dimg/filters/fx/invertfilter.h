/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-17-07
 * Description : An invert image threaded image filter.
 *
 * Copyright (C) 2005-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#ifndef INVERTFILTER_H
#define INVERTFILTER_H

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"

namespace Digikam
{

class DIGIKAM_EXPORT InvertFilter : public DImgThreadedFilter
{

public:

    explicit InvertFilter(DImg* orgImage, QObject* parent=0);

    // Constructor for slave mode: execute immediately in current thread with specified master filter
    explicit InvertFilter(DImgThreadedFilter* parentFilter, const DImg& orgImage, const DImg& destImage,
                          int progressBegin=0, int progressEnd=100);

    ~InvertFilter(){};

    static QString          FilterIdentifier() { return "digikam:InvertFilter"; }
    static QString          DisplayableName() { return "Invert Effect"; }
    static QList<int>       SupportedVersions() { return QList<int>() << 1; }
    static int              CurrentVersion() { return 1; }
    
    virtual QString         filterIdentifier() const { return FilterIdentifier(); }
    virtual FilterAction    filterAction();
    void                    readParameters(const FilterAction& action);

private:

    void filterImage();
};

}  // namespace Digikam

#endif /* INVERTFILTER_H */
