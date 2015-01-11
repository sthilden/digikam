/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2010-12-26
 * Description : images versions tree view overlays
 *
 * Copyright (C) 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
 * Copyright (C) 2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "versionsoverlays.h"

// Qt includes

#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "imageinfo.h"
#include "imagehistorygraphmodel.h"
#include "imagelistmodel.h"
#include "itemviewhoverbutton.h"
#include "tagscache.h"
#include "versionmanagersettings.h"

namespace Digikam
{

class ShowHideVersionsOverlay::Button : public ItemViewHoverButton
{
public:

    explicit Button(QAbstractItemView* parentView);
    virtual QSize sizeHint() const;

protected:

    virtual QPixmap icon();
    virtual void updateToolTip();
};

ShowHideVersionsOverlay::Button::Button(QAbstractItemView* parentView)
    : ItemViewHoverButton(parentView)
{
}

QSize ShowHideVersionsOverlay::Button::sizeHint() const
{
    return QSize(16, 16);
}

QPixmap ShowHideVersionsOverlay::Button::icon()
{
    const char* icon = isChecked() ? "edit-bomb" : "edit-clear-history";
    //const char* icon = isChecked() ? "layer-visible-off" : "layer-visible-on";
    return QIcon::fromTheme(icon).pixmap(16);
}

void ShowHideVersionsOverlay::Button::updateToolTip()
{
    setToolTip(isChecked() ?
               i18nc("@info:tooltip", "Hide item permanently") :
               i18nc("@info:tooltip", "Show item permanently"));
}

// ------------------------------------------------------------------------------------

ShowHideVersionsOverlay::ShowHideVersionsOverlay(QObject* const parent)
    : HoverButtonDelegateOverlay(parent)
{
}

void ShowHideVersionsOverlay::setSettings(const VersionManagerSettings& settings)
{
    m_filter = VersionImageFilterSettings(settings);
}

void ShowHideVersionsOverlay::setActive(bool active)
{
    HoverButtonDelegateOverlay::setActive(active);

    if (active)
    {
        connect(button(), SIGNAL(clicked(bool)),
                this, SLOT(slotClicked(bool)));
    }
    else
    {
        // button is deleted
    }
}

ItemViewHoverButton* ShowHideVersionsOverlay::createButton()
{
    return new Button(view());
}

void ShowHideVersionsOverlay::updateButton(const QModelIndex& index)
{
    const QRect rect = m_view->visualRect(index);
    const QSize size = button()->size();

    const int gap    = 5;
    const int x      = rect.right() - gap - size.width();
    const int y      = rect.bottom() - gap - size.height();
    button()->move(QPoint(x, y));

    ImageInfo info   = ImageModel::retrieveImageInfo(index);
    button()->setChecked(m_filter.isExemptedBySettings(info));
}

void ShowHideVersionsOverlay::slotClicked(bool checked)
{
    QModelIndex index = button()->index();

    if (index.isValid())
    {
        ImageInfo info = ImageModel::retrieveImageInfo(index);
        int tagId      = TagsCache::instance()->getOrCreateInternalTag(InternalTagName::versionAlwaysVisible());

        if (checked)
        {
            info.setTag(tagId);
        }
        else
        {
            info.removeTag(tagId);
        }
    }
}

bool ShowHideVersionsOverlay::checkIndex(const QModelIndex& index) const
{
    if (index.data(ImageHistoryGraphModel::IsImageItemRole).toBool())
    {
        ImageInfo info = ImageModel::retrieveImageInfo(index);
        return m_filter.isHiddenBySettings(info);
    }

    return false;
}

// --------------------------------------------------------------------

class ActionVersionsOverlay::Button : public ItemViewHoverButton
{
public:

    Button(QAbstractItemView* const parentView, const KGuiItem& gui);
    virtual QSize sizeHint() const;

protected:

    virtual QPixmap icon();
    virtual void updateToolTip();

protected:

    KGuiItem gui;
};

ActionVersionsOverlay::Button::Button(QAbstractItemView* const parentView, const KGuiItem& gui)
    : ItemViewHoverButton(parentView), gui(gui)
{
}

QSize ActionVersionsOverlay::Button::sizeHint() const
{
    return QSize(16, 16);
}

QPixmap ActionVersionsOverlay::Button::icon()
{
    return QIcon::fromTheme(gui.iconName()).pixmap(16);
}

void ActionVersionsOverlay::Button::updateToolTip()
{
    setToolTip(gui.toolTip());
}

// ------------------------------------------------------------------------------------------

ActionVersionsOverlay::ActionVersionsOverlay(QObject* const parent, const KGuiItem& gui)
    : HoverButtonDelegateOverlay(parent),
      m_gui(gui), m_referenceModel(0)
{
}

ActionVersionsOverlay::Button* ActionVersionsOverlay::button() const
{
    return static_cast<Button*>(HoverButtonDelegateOverlay::button());
}

void ActionVersionsOverlay::setReferenceModel(const ImageModel* model)
{
    m_referenceModel = model;
}

void ActionVersionsOverlay::setActive(bool active)
{
    HoverButtonDelegateOverlay::setActive(active);

    if (active)
    {
        connect(button(), SIGNAL(clicked(bool)),
                this, SLOT(slotClicked(bool)));
    }
    else
    {
        // button is deleted
    }
}

ItemViewHoverButton* ActionVersionsOverlay::createButton()
{
    return new Button(view(), m_gui);
}

void ActionVersionsOverlay::updateButton(const QModelIndex& index)
{
    const QRect rect = m_view->visualRect(index);
    const QSize size = button()->size();

    const int gap    = 5;
    const int x      = rect.right() - gap - size.width();
    const int y      = rect.top() + gap;
    button()->move(QPoint(x, y));
}

void ActionVersionsOverlay::slotClicked(bool checked)
{
    Q_UNUSED(checked);
    QModelIndex index = button()->index();

    if (index.isValid())
    {
        emit activated(ImageModel::retrieveImageInfo(index));
    }
}

bool ActionVersionsOverlay::checkIndex(const QModelIndex& index) const
{
    if (index.data(ImageHistoryGraphModel::IsImageItemRole).toBool())
    {
        if (m_referenceModel)
        {
            ImageInfo info = ImageModel::retrieveImageInfo(index);
            // show overlay if image is not contained in reference model
            return !m_referenceModel->hasImage(info);
        }

        return true;
    }

    return false;
}

} // namespace Digikam
