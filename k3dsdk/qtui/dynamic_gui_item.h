#ifndef K3DSDK_QTUI_DYNAMIC_GUI_ITEM_H
#define K3DSDK_QTUI_DYNAMIC_GUI_ITEM_H

// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
	\author Bart Janssens
*/

#include <k3dsdk/qtui/gui_element.h>

#include <QQmlListProperty>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// dynamic_gui_item

/// Root for a set of K-3D widgets
class dynamic_gui_item :
	public QQuickItem, public gui_element
{
	Q_OBJECT
	Q_CLASSINFO("qml_type_name", "DynamicGuiItem")
	Q_PROPERTY(bool highlighted READ highlighted WRITE set_highlighted NOTIFY on_highlighted_changed)
	Q_PROPERTY(bool onTop READ on_top WRITE set_on_top NOTIFY on_top_changed)
	Q_PROPERTY(QQmlListProperty<QQuickItem> overlappingParents READ overlapping_parents NOTIFY overlapping_parents_changed)
public:
	dynamic_gui_item(QQuickItem *parent = 0);
	~dynamic_gui_item();
	virtual void componentComplete();

	bool highlighted() const
	{
		return m_highlighted;
	}

	bool on_top() const
	{
		return m_on_top;
	}

	void set_highlighted(const bool Highlighted);
	void set_on_top(const bool OnTop);

	QQmlListProperty<QQuickItem> overlapping_parents();

	const QList<QQuickItem*>& overlapping_parents_list() const;

	/// Rebuild the list of overlapping parent items
	void rebuild_overlapping_parents();

	/// Clear all children from the current item
	Q_INVOKABLE void clear();

signals:
	void on_highlighted_changed(bool);
	void on_top_changed(bool);
	void overlapping_parents_changed(QQmlListProperty<QQuickItem>);

protected:
	virtual void itemChange(ItemChange change, const ItemChangeData& value);
	virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);
	bool m_highlighted = false;
	bool m_on_top = true;
	QList<QQuickItem*> m_overlapping_parents;
	QQuickItem* m_default_buttons = nullptr; // Buttons shown when nothing is loaded
	QQuickItem* m_decoration = nullptr; // Menu and so on
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_DYNAMIC_GUI_ITEM_H

