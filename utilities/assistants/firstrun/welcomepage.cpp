/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-28-04
 * Description : first run assistant dialog
 *
 * Copyright (C) 2009-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "welcomepage.h"

// Qt includes

#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dwidgetutils.h"
#include "digikam_version.h"

namespace Digikam
{

WelcomePage::WelcomePage(FirstRunDlg* const dlg)
    : FirstRunDlgPage(dlg, i18n("Welcome to digiKam %1", QLatin1String(digikam_version_short)))
{
    DVBox* const vbox   = new DVBox(this);
    QLabel* const title = new QLabel(vbox);
    title->setWordWrap(true);
    title->setText(i18n("<qt>"
                        "<p><h1><b>Welcome to digiKam %1</b></h1></p>"
                        "<p>digiKam is an advanced digital photo management application published "
                        "as open-source.</p>"
                        "<p>This assistant will help you to configure first run settings to be able to "
                        "use digiKam quickly.</p>"
                        "</qt>", QLatin1String(digikam_version_short)));

    setPageWidget(vbox);
}

WelcomePage::~WelcomePage()
{
}

}   // namespace Digikam
