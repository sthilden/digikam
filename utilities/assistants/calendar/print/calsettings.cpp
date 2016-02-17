/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-11-03
 * Description : calendar parameters.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2008 by Orgad Shaneh <orgads at gmail dot com>
 * Copyright (C) 2011      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012      by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "calsettings.h"

// KDE includes

#include <klocalizedstring.h>
#include <kcalendarsystem.h>

// KCalCore includes

#include <kcalcore/icalformat.h>
#include <kcalcore/filestorage.h>
#include <kcalcore/memorycalendar.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

QPointer<CalSettings> CalSettings::s_instance;

CalSettings::CalSettings(QObject* const parent)
    : QObject(parent)
{
    params.year = KLocale::global()->calendar()->earliestValidDate().year() + 1;
    setPaperSize(QString::fromLatin1("A4"));
    setResolution(QString::fromLatin1("High"));
    setImagePos(0);
}

CalSettings::~CalSettings()
{
}

CalSettings* CalSettings::instance(QObject* const parent)
{
    if (s_instance.isNull())
    {
        s_instance = new CalSettings(parent);
    }

    return s_instance;
}

void CalSettings::setYear(int year)
{
    params.year = year;
    emit settingsChanged();
}

int CalSettings::year() const
{
    return params.year;
}

void CalSettings::setImage(int month, const QUrl& path)
{
    m_monthMap.insert(month, path);
}

QUrl CalSettings::image(int month) const
{
    return m_monthMap.contains(month) ? m_monthMap[month] : QUrl();
}

void CalSettings::setPaperSize(const QString& paperSize)
{
    if (paperSize == QString::fromLatin1("A4"))
    {
        params.paperWidth  = 210;
        params.paperHeight = 297;
        params.pageSize    = QPrinter::A4;
    }
    else if (paperSize == QString::fromLatin1("US Letter"))
    {
        params.paperWidth  = 216;
        params.paperHeight = 279;
        params.pageSize    = QPrinter::Letter;
    }

    emit settingsChanged();
}

void CalSettings::setResolution(const QString& resolution)
{
    if (resolution == QString::fromLatin1("High"))
    {
        params.printResolution = QPrinter::HighResolution;
    }
    else if (resolution == QString::fromLatin1("Low"))
    {
        params.printResolution = QPrinter::ScreenResolution;
    }

    emit settingsChanged();
}

void CalSettings::setImagePos(int pos)
{
    const int previewSize = 300;

    switch (pos)
    {
        case CalParams::Top:
        {
            float zoom    = qMin((float)previewSize / params.paperWidth,
                                 (float)previewSize / params.paperHeight);
            params.width  = (int)(params.paperWidth  * zoom);
            params.height = (int)(params.paperHeight * zoom);

            params.imgPos = CalParams::Top;
            break;
        }

        case CalParams::Left:
        {
            float zoom    = qMin((float)previewSize / params.paperWidth,
                                 (float)previewSize / params.paperHeight);
            params.width  = (int)(params.paperHeight  * zoom);
            params.height = (int)(params.paperWidth   * zoom);

            params.imgPos = CalParams::Left;
            break;
        }

        default:
        {
            float zoom    = qMin((float)previewSize / params.paperWidth,
                                 (float)previewSize / params.paperHeight);
            params.width  = (int)(params.paperHeight  * zoom);
            params.height = (int)(params.paperWidth   * zoom);

            params.imgPos = CalParams::Right;
            break;
        }
    }

    emit settingsChanged();
}

void CalSettings::setDrawLines(bool draw)
{
    if (params.drawLines != draw)
    {
        params.drawLines = draw;
        emit settingsChanged();
    }
}

void CalSettings::setRatio(int ratio)
{
    if (params.ratio != ratio)
    {
        params.ratio = ratio;
        emit settingsChanged();
    }
}

void CalSettings::setFont(const QString& font)
{
    if (params.baseFont.family() != font)
    {
        params.baseFont = QFont(font);
        emit settingsChanged();
    }
}

void CalSettings::clearSpecial()
{
    m_special.clear();
}

void CalSettings::addSpecial(const QDate& date, const Day& info)
{
    if (m_special.contains(date))
    {
        m_special[date].second.append(QString::fromLatin1("; ")).append(info.second);
    }
    else
    {
        m_special[date] = info;
    }
}

void CalSettings::loadSpecial(const QUrl& url, const QColor& color)
{
    if (url.isEmpty())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Loading calendar from file failed: No valid url provided!";
        return;
    }

    KCalCore::MemoryCalendar::Ptr memCal(new KCalCore::MemoryCalendar(QString::fromLatin1("UTC")));
    KCalCore::FileStorage::Ptr fileStorage(new KCalCore::FileStorage(memCal, url.path(), new KCalCore::ICalFormat));

    qCDebug(DIGIKAM_GENERAL_LOG) << "Loading calendar from file " << url.path();

    if (!fileStorage->load())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Failed!";
    }
    else
    {
        QDate qFirst, qLast;
        KLocale::global()->calendar()->setDate(qFirst, params.year, 1, 1);
        KLocale::global()->calendar()->setDate(qLast, params.year + 1, 1, 1);
        qLast = qLast.addDays(-1);
        KDateTime dtFirst(qFirst);
        KDateTime dtLast(qLast);
        KDateTime dtCurrent;

        int counter                = 0;
        KCalCore::Event::List list = memCal->rawEvents(qFirst, qLast);

        foreach(const KCalCore::Event::Ptr event, list)
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << event->summary() << endl << "--------";
            counter++;

            if (event->recurs())
            {
                KCalCore::Recurrence* recur = event->recurrence();

                for (dtCurrent = recur->getNextDateTime(dtFirst.addDays(-1));
                     (dtCurrent <= dtLast) && dtCurrent.isValid();
                     dtCurrent = recur->getNextDateTime(dtCurrent))
                {
                    addSpecial(dtCurrent.date(), Day(color, event->summary()));
                }
            }
            else
            {
                addSpecial(event->dtStart().date(), Day(color, event->summary()));
            }
        }

        qCDebug(DIGIKAM_GENERAL_LOG) << "Loaded " << counter << " events";
        memCal->close();
        fileStorage->close();
    }
}

bool CalSettings::isPrayDay(const QDate& date) const
{
    return (date.dayOfWeek() == KLocale::global()->weekDayOfPray());
}

/*!
    \returns true if m_special formatting is to be applied to the particular day
 */
bool CalSettings::isSpecial(int month, int day) const
{
    QDate dt;
    KLocale::global()->calendar()->setDate(dt, params.year, month, day);

    return (isPrayDay(dt) || m_special.contains(dt));
}

/*!
    \returns the color to be used for painting of the day info
 */
QColor CalSettings::getDayColor(int month, int day) const
{
    QDate dt;
    KLocale::global()->calendar()->setDate(dt, params.year, month, day);

    if (isPrayDay(dt))
    {
        return Qt::red;
    }

    if (m_special.contains(dt))
    {
        return m_special[dt].first;
    }

    //default
    return Qt::black;
}

/*!
    \returns the description of the day to be painted on the calendar.
 */
QString CalSettings::getDayDescr(int month, int day) const
{
    QDate dt;
    KLocale::global()->calendar()->setDate(dt, params.year, month, day);

    QString ret;

    if (m_special.contains(dt))
    {
        ret = m_special[dt].second;
    }

    return ret;
}

QPrinter::PrinterMode CalSettings::resolution() const
{
    return params.printResolution;
}

}  // Namespace Digikam