/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * Copyright (C) 2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "maintenancedlg.moc"

// Qt includes

#include <QHeaderView>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QTreeWidget>

// KDE includes

#include <klocale.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <ksqueezedtextlabel.h>

namespace Digikam
{

class MaintenanceDlg::MaintenanceDlgPriv
{
public:

    MaintenanceDlgPriv() :
        logo(0)
    {
    }

    QLabel*             logo;
};

MaintenanceDlg::MaintenanceDlg(QWidget* parent)
    : KDialog(parent), d(new MaintenanceDlgPriv)
{
    setCaption(i18n("Maintenance"));
    setButtons(Ok|Help|Cancel);
    setDefaultButton(Cancel);

    QWidget* page     = new QWidget(this);
    setMainWidget(page);

    QGridLayout* grid = new QGridLayout(page);

    d->logo           = new QLabel(page);
    d->logo->setPixmap(QPixmap(KStandardDirs::locate("data", "digikam/data/logo-digikam.png"))
                       .scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    grid->addWidget(d->logo,        0, 0, 3, 1);
    grid->setSpacing(spacingHint());
    grid->setMargin(0);
    grid->setColumnStretch(2, 10);

    setInitialSize(QSize(500, 150));
}

MaintenanceDlg::~MaintenanceDlg()
{
    delete d;
}

}  // namespace Digikam
