/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-02-09
 * Description : a plugin to apply Blur FX to images
 *
 * Copyright 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef IMAGEEFFECT_BLURFX_H
#define IMAGEEFFECT_BLURFX_H

// Digikam includes.

#include "ctrlpaneldlg.h"

class QLabel;

namespace KDcrawIface
{
class RIntNumInput;
class RComboBox;
}

namespace DigikamBlurFXImagesPlugin
{

class ImageEffect_BlurFX : public Digikam::CtrlPanelDlg
{
    Q_OBJECT

public:

    ImageEffect_BlurFX(QWidget *parent);
    ~ImageEffect_BlurFX();

private slots:

    void slotEffectTypeChanged(int type);
    void readUserSettings();

private:

    void writeUserSettings();
    void resetValues();
    void prepareEffect();
    void prepareFinal();
    void abortPreview();
    void putPreviewData();
    void putFinalData();
    void renderingFinished();

private:

    QLabel       *m_effectTypeLabel;
    QLabel       *m_distanceLabel;
    QLabel       *m_levelLabel;

    KDcrawIface::RComboBox    *m_effectType;

    KDcrawIface::RIntNumInput *m_distanceInput;
    KDcrawIface::RIntNumInput *m_levelInput;
};

}  // NameSpace DigikamBlurFXImagesPlugin

#endif /* IMAGEEFFECT_BLURFX_H */
