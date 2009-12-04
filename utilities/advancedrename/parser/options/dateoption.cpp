/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-08-08
 * Description : an option to provide date information to the parser
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "dateoption.moc"

// Qt includes

#include <QDateTime>
#include <QPointer>
#include <QTimer>

// KDE includes

#include <kiconloader.h>
#include <klocale.h>

// Local includes

#include "ui_dateoptiondialogwidget.h"

namespace Digikam
{

const QString dateFormatLinkDescr = i18nc("date format settings", "format settings");
const QString dateFormatLink      = QString("<a href='http://doc.trolltech.com/latest/qdatetime.html#toString'>%1</a>")
                                           .arg(dateFormatLinkDescr);

// --------------------------------------------------------

DateFormat::DateFormat()
{
    m_map.clear();

    m_map.insert(Standard, DateFormatDescriptor(QString("Standard"), QString("yyyyMMddThhmmss")));
    m_map.insert(ISO,      DateFormatDescriptor(QString("ISO"),      Qt::ISODate));
    m_map.insert(FullText, DateFormatDescriptor(QString("Text"),     Qt::TextDate));
    m_map.insert(Locale,   DateFormatDescriptor(QString("Locale"),   Qt::SystemLocaleShortDate));
    m_map.insert(Custom,   DateFormatDescriptor(QString("Custom"),   QString("")));
}

QString DateFormat::identifier(Type type)
{
    DateFormatDescriptor desc = m_map.at((int)type);
    return desc.first;
}

QVariant DateFormat::formatType(Type type)
{
    DateFormatDescriptor desc = m_map.at((int)type);
    return desc.second;
}

QVariant DateFormat::formatType(QString identifier)
{
    QVariant v;
    foreach (const DateFormatDescriptor& desc, m_map)
    {
        if (desc.first == identifier)
        {
            v = desc.second;
            break;
        }
    }

    if (identifier.isEmpty())
    {
        return m_map.at(Standard).second;
    }

    return v;
}

// --------------------------------------------------------

DateOptionDialog::DateOptionDialog(ParseObject* parent)
                : ParseObjectDialog(parent), ui(new Ui::DateOptionDialogWidget)
{
    QWidget* mainWidget = new QWidget(this);
    ui->setupUi(mainWidget);

    // --------------------------------------------------------

    // fill the date source combobox
    ui->dateSourcePicker->addItem(i18nc("Get date information from the image", "Image"),
                                  QVariant(FromImage));
//    ui->dateSourcePicker->addItem(i18nc("Get date information from the current date", "Current Date"),
//                                  QVariant(CurrentDateTime));
    ui->dateSourcePicker->addItem(i18nc("Set a fixed date", "Fixed Date"),
                                  QVariant(FixedDateTime));

    // fill the date format combobox
    DateFormat df;
    foreach (const DateFormat::DateFormatDescriptor& desc, df.map())
    {
        ui->dateFormatPicker->addItem(desc.first);
    }

    // set the datePicker and timePicker to the current local datetime
    QDateTime currentDateTime = QDateTime::currentDateTime();
    ui->datePicker->setDate(currentDateTime.date());
    ui->timePicker->setTime(currentDateTime.time());

    ui->dateFormatLink->setOpenExternalLinks(true);
    ui->dateFormatLink->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::LinksAccessibleByKeyboard);
    ui->dateFormatLink->setText(dateFormatLink);

    ui->customFormatInput->setClickMessage(i18n("Enter custom format"));

    // --------------------------------------------------------

    connect(ui->dateSourcePicker, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotDateSourceChanged(int)));

    connect(ui->dateFormatPicker, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotDateFormatChanged(int)));

    connect(ui->customFormatInput, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotCustomFormatChanged(const QString&)));

    // --------------------------------------------------------

    ui->dateFormatPicker->setCurrentIndex(DateFormat::Standard);
    slotDateFormatChanged(ui->dateFormatPicker->currentIndex());

    // --------------------------------------------------------

    setSettingsWidget(mainWidget);
}

DateOptionDialog::~DateOptionDialog()
{
    delete ui;
}

DateOptionDialog::DateSource DateOptionDialog::dateSource()
{
    QVariant v = ui->dateSourcePicker->itemData(ui->dateSourcePicker->currentIndex());
    bool ok    = true;
    int choice = v.toInt(&ok);

    return (DateSource)choice;
}

QString DateOptionDialog::formattedDateTime(const QDateTime& date)
{
    if (ui->dateFormatPicker->currentIndex() == DateFormat::Custom)
    {
        return date.toString(ui->customFormatInput->text());
    }

    DateFormat df;
    QVariant v;

    v = df.formatType((DateFormat::Type)ui->dateFormatPicker->currentIndex());
    QString tmp;
    if (v.type() == QVariant::String)
    {
        tmp = date.toString(v.toString());
    }
    else
    {
        tmp = date.toString((Qt::DateFormat)v.toInt());
    }
    return tmp;
}

void DateOptionDialog::slotDateSourceChanged(int index)
{
    Q_UNUSED(index)

    DateSource choice = dateSource();
    ui->fixedDateContainer->setEnabled( (choice == FixedDateTime) );
}

void DateOptionDialog::slotDateFormatChanged(int index)
{
    bool custom = (index == DateFormat::Custom);

    ui->customFormatInput->setEnabled(custom);

    ui->dateFormatLink->setEnabled(custom);
    ui->dateFormatLink->setVisible(custom);

    updateExampleLabel();
}

void DateOptionDialog::slotCustomFormatChanged(const QString&)
{
    updateExampleLabel();
}

void DateOptionDialog::updateExampleLabel()
{
    QString tmp = QString("example: %1").arg(formattedDateTime(QDateTime::currentDateTime()));
    ui->exampleLabel->setText(tmp);
}

// --------------------------------------------------------

DateOption::DateOption()
          : Option(i18n("Date && Time..."),
                   i18n("Add date and time information"),
                   SmallIcon("view-pim-calendar"))
{
    addToken("[date]",          i18n("Date and time (standard format)"));
    addToken("[date:|key|]",    i18n("Date and time (|key| = ISO/Text/Locale)"));
    addToken("[date:|format|]", i18n("Date and time") + " (" +  dateFormatLink + ')');

    QRegExp reg("\\[date(:.*)?\\]");
    reg.setMinimal(true);
    setRegExp(reg);
}

QString DateOption::parseOperation(const QRegExp& regExp, ParseSettings& settings)
{
    QString tmp;
    DateFormat df;

    QString token = regExp.cap(1);
    if (!token.isEmpty())
    {
        token.remove(0, 1);
    }

    QVariant v = df.formatType(token);
    if (v.isNull())
    {
        tmp = settings.dateTime.toString(token);
    }
    else
    {
        if (v.type() == QVariant::String)
        {
            tmp = settings.dateTime.toString(v.toString());
        }
        else
        {
            tmp = settings.dateTime.toString((Qt::DateFormat)v.toInt());
        }
    }

    return tmp;
}

void DateOption::slotTokenTriggered(const QString& token)
{
    Q_UNUSED(token)

    QVariant v;
    DateFormat df;
    QString dateString;

    QString tokenStr               = QString("[date:%1]");
    QPointer<DateOptionDialog> dlg = new DateOptionDialog(this);

    if (dlg->exec() == KDialog::Accepted)
    {
        int index = dlg->ui->dateFormatPicker->currentIndex();

        // use custom date format?
        if (dlg->dateSource() == DateOptionDialog::FixedDateTime)
        {
            QDateTime date;
            date.setDate(dlg->ui->datePicker->date());
            date.setTime(dlg->ui->timePicker->time());

            v = (index == DateFormat::Custom) ? dlg->ui->customFormatInput->text()
                                              : df.formatType((DateFormat::Type)index);

            if (v.type() == QVariant::String)
            {
                dateString = date.toString(v.toString());
            }
            else
            {
                dateString = date.toString((Qt::DateFormat)v.toInt());
            }
        }
        else        // use predefined keywords for date formatting
        {
            switch (index)
            {
                case DateFormat::Standard:
                    dateString = tokenStr.arg(QString(""));
                    dateString.remove(':');
                    break;
                case DateFormat::Custom:
                    dateString = tokenStr.arg(dlg->ui->customFormatInput->text());
                    break;
                default:
                    QString identifier = df.identifier((DateFormat::Type) index);
                    dateString         = tokenStr.arg(identifier);
                    break;
            }
        }
    }

    delete dlg;
    emit signalTokenTriggered(dateString);
}

} // namespace Digikam
