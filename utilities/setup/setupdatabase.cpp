/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : database setup tab
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * Copyright (C) 2012-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "setupdatabase.h"

// Qt includes

#include <QCursor>
#include <QGroupBox>
#include <QLabel>
#include <QDir>
#include <QList>
#include <QFileInfo>
#include <QGridLayout>
#include <QHelpEvent>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QSpinBox>
#include <QFormLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>
#include <QUrl>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "applicationsettings.h"
#include "dbsettingswidget.h"
#include "dbengineparameters.h"
#include "databaseserverstarter.h"
#include "scancontroller.h"
#include "coredbschemaupdater.h"

namespace Digikam
{

class SetupDatabase::Private
{
public:

    Private() :
        databaseWidget(0),
        updateBox(0),
        hashesButton(0)
    {
    }

    DatabaseSettingsWidget* databaseWidget;
    QGroupBox*              updateBox;
    QPushButton*            hashesButton;
};

SetupDatabase::SetupDatabase(QWidget* const parent)
    : QScrollArea(parent),
      d(new Private)
{
    QWidget* const page     = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;

    d->databaseWidget       = new DatabaseSettingsWidget;
    mainLayout->addWidget(d->databaseWidget);

    if (!CoreDbSchemaUpdater::isUniqueHashUpToDate())
    {
        d->updateBox                    = new QGroupBox(i18nc("@title:group", "Updates"));
        QGridLayout* const updateLayout = new QGridLayout;

        d->hashesButton                 = new QPushButton(i18nc("@action:button", "Update File Hashes"));
        d->hashesButton->setWhatsThis(i18nc("@info:tooltip",
                                            "<qt>File hashes are used to identify identical files and to display thumbnails. "
                                            "A new, improved algorithm to create the hash is now used. "
                                            "The old algorithm, though, still works quite well, so it is recommended to "
                                            "carry out this upgrade, but not required.<br> "
                                            "After the upgrade you cannot use your database with a digiKam version "
                                            "prior to 2.0.</qt>"));

        QPushButton* const infoHash     = new QPushButton;
        infoHash->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
        infoHash->setToolTip(i18nc("@info:tooltip", "Get information about <interface>Update File Hashes</interface>"));

        updateLayout->addWidget(d->hashesButton, 0, 0);
        updateLayout->addWidget(infoHash,        0, 1);
        updateLayout->setColumnStretch(2, 1);

        d->updateBox->setLayout(updateLayout);

        mainLayout->addStretch(10);
        mainLayout->addWidget(d->updateBox);

        connect(d->hashesButton, SIGNAL(clicked()),
                this, SLOT(upgradeUniqueHashes()));

        connect(infoHash, SIGNAL(clicked()),
                this, SLOT(showHashInformation()));
    }

    page->setLayout(mainLayout);
    setWidget(page);
    setWidgetResizable(true);

    // --------------------------------------------------------

    readSettings();
    adjustSize();
}

SetupDatabase::~SetupDatabase()
{
    delete d;
}

void SetupDatabase::applySettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    switch(d->databaseWidget->databaseType())
    {
        case DatabaseSettingsWidget::SQlite:
        {
            QString newPath = d->databaseWidget->databasePath();
            QDir oldDir(d->databaseWidget->orgDatabasePrm().getCoreDatabaseNameOrDir());
            QDir newDir(newPath);

            if (oldDir != newDir || d->databaseWidget->databaseBackend() != d->databaseWidget->orgDatabasePrm().databaseType)
            {
                settings->setDbEngineParameters(d->databaseWidget->getDbEngineParameters());
                settings->saveSettings();
                DatabaseServerStarter::stopServerManagerProcess();
            }
            break;
        }
        case DatabaseSettingsWidget::MysqlInternal:
        {
            settings->setDbEngineParameters(d->databaseWidget->getDbEngineParameters());
            settings->saveSettings();
            DatabaseServerStarter::startServerManagerProcess(d->databaseWidget->databaseBackend());
            break;
        }
        default: // DatabaseSettingsWidget::MysqlServer
        {
            settings->setDbEngineParameters(d->databaseWidget->getDbEngineParameters());
            settings->saveSettings();
            DatabaseServerStarter::stopServerManagerProcess();
            break;
        }
    }
}

void SetupDatabase::readSettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    d->databaseWidget->setParametersFromSettings(settings);
}

void SetupDatabase::upgradeUniqueHashes()
{
    int result = QMessageBox::warning(this, qApp->applicationName(),
                                      i18nc("@info",
                                            "<p>The process of updating the file hashes takes a few minutes.</p> "
                                            "<p>Please ensure that any important collections on removable media are connected. "
                                            "<i>After the upgrade you cannot use your database with a digiKam version "
                                            "prior to 2.0.</i></p> "
                                            "<p>Do you want to begin the update?</p>"));

    if (result == QMessageBox::Yes)
    {
        ScanController::instance()->updateUniqueHash();
    }
}

void SetupDatabase::showHashInformation()
{
    qApp->postEvent(d->hashesButton, new QHelpEvent(QEvent::WhatsThis, QPoint(0, 0), QCursor::pos()));
}

}  // namespace Digikam
