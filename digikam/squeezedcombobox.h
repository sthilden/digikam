////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005 Tom Albers <tomalbers@kde.nl>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301  USA
//
//////////////////////////////////////////////////////////////////////////////

/** @file squeezedcombobox.h */

#ifndef SQUEEZEDCOMBOBOX_H
#define SQUEEZEDCOMBOBOX_H

class QTimer;
class SqueezedComboBox;

#include <qcombobox.h>
#include <qtooltip.h>

/** @class UnSqueezedTip
 * This class shows a tooltip for a SqueezedComboBox
 * the tooltip will contain the full text and helps
 * the user find the correct entry. It is automatically
 * activated when starting a SqueezedComboBox. This is
 * inherited from QToolTip
 * 
 * @author Tom Albers
 */
class UnSqueezedTip : public QToolTip
{
public:
    /**
     * Constructor. An example call (as done in
     * SqueezedComboBox::SqueezedComboBox):
     * @code
     * t = new UnSqueezedTip( this->listBox()->viewport(), this );
     * @endcode
     * 
     * @param parent parent widget (viewport)
     * @param name parent widget
     */
    UnSqueezedTip( QWidget *parent, SqueezedComboBox *name );

protected:
    /**
     * Reimplemented version from QToolTip which shows the
     * tooltip when needed.
     * @param  pos the point where the mouse currently is
     */
    void maybeTip( const QPoint& pos );

private:
    SqueezedComboBox*        m_originalWidget;
};

/** @class SqueezedComboBox
 *
 * This widget is a QComboBox, but then a little bit
 * different. It only shows the right part of the items
 * depending on de size of the widget. When it is not
 * possible to show the complete item, it will be shortened
 * and "..." will be prepended.
 *
 * @author Tom Albers
 */
class SqueezedComboBox : public QComboBox
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent parent widget
     * @param name name to give to the widget
     */
    SqueezedComboBox(QWidget *parent = 0, const char *name = 0 );

    /**
     * destructor
     */
    virtual ~SqueezedComboBox();

    /**
     * This inserts a item to the list. See QComboBox::insertItem()
     * for detaills. Please do not use QComboBox::insertItem() to this
     * widget, as that will fail.
     * @param newItem the original (long version) of the item which needs
     *                to be added to the combobox
     * @param index the position in the widget.
     */
    void insertSqueezedItem(const QString& newItem, int index);

    /**
     * This method returns the full text (not squeezed) of the currently
     * highlighted item.
     * @return full text of the highlighted item
     */
    QString itemHighlighted( );

    /**
     * Sets the sizeHint() of this widget.
     */
    virtual QSize sizeHint() const;

private slots:
    void slotTimeOut();
    void slotUpdateToolTip( int index );

private:
    void resizeEvent ( QResizeEvent * );
    QString squeezeText( const QString& original);

    QMap<int,QString>   m_originalItems;
    QTimer*             m_timer;
    UnSqueezedTip*      m_tooltip;
};


#endif
