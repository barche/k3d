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

#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>

#include <k3dsdk/qtui/dynamic_gui_item.h>
#include <k3dsdk/qtui/utility.h>

k3d::qtui::qml_type_registrator<k3d::qtui::dynamic_gui_item> register_dynamic_gui_item;

namespace k3d
{

namespace qtui
{

namespace detail
{

// QQmlLstProperty callbacks
QQuickItem* overlapping_parents_at(QQmlListProperty<QQuickItem>* prop, int index)
{
	dynamic_gui_item* item = static_cast<dynamic_gui_item*>(prop->object);
	return item->overlapping_parents_list().at(index);
}
int overlapping_parents_count(QQmlListProperty<QQuickItem> *prop)
{
	dynamic_gui_item* item = static_cast<dynamic_gui_item*>(prop->object);
	return item->overlapping_parents_list().count();
}

// Recurse over child items to rebuild parents list
void recursively_rebuild_overlapping_parents(QQuickItem* Item)
{
	for(QQuickItem* child: Item->childItems())
	{
		dynamic_gui_item* dynamic_child = dynamic_cast<dynamic_gui_item*>(child);
		if(dynamic_child != nullptr)
		{
			dynamic_child->rebuild_overlapping_parents();
		}
		recursively_rebuild_overlapping_parents(child);
	}
}

} // namespace detail

dynamic_gui_item::dynamic_gui_item(QQuickItem *parent) : QQuickItem(parent)
{
}

dynamic_gui_item::~dynamic_gui_item()
{
}

void dynamic_gui_item::componentComplete()
{
	QQuickItem::componentComplete();

	if(childItems().size() == 0)
	{
		m_default_buttons = load_qml(QUrl(QStringLiteral("qrc:/qml/detail/dynamic_gui_item_mousearea.qml")), this);
		return_if_fail(m_default_buttons != nullptr);
	}
	else
	{
		for(QQuickItem* child_item : childItems())
		{
			igui_element* child_elem = dynamic_cast<igui_element*>(child_item);
			if(child_elem != nullptr)
			{
				register_child(child_elem);
			}
		}
	}

	m_decoration = load_qml(QUrl(QStringLiteral("qrc:/qml/detail/dynamic_gui_item_decoration.qml")), this);
	return_if_fail(m_decoration != nullptr);
}

void dynamic_gui_item::set_highlighted(const bool Highlighted)
{
	m_highlighted = Highlighted;
	emit on_highlighted_changed(m_highlighted);
}

void dynamic_gui_item::set_on_top(const bool OnTop)
{
	m_on_top = OnTop;
	emit on_top_changed(m_on_top);
}

QQmlListProperty<QQuickItem> dynamic_gui_item::overlapping_parents()
{
	return QQmlListProperty<QQuickItem>(this, 0, detail::overlapping_parents_count, detail::overlapping_parents_at);
}

const QList<QQuickItem*>& dynamic_gui_item::overlapping_parents_list() const
{
	return m_overlapping_parents;
}

void dynamic_gui_item::rebuild_overlapping_parents()
{
	m_overlapping_parents.clear();
	m_overlapping_parents.push_back(this);
	set_on_top(true);

	QQuickItem* parent = parentItem();
	qreal my_right = x() + width();
	qreal my_top = y();
	while(parent != nullptr)
	{
		my_right += parent->x();
		my_top += parent->y();
		dynamic_gui_item* parent_gui_item = dynamic_cast<dynamic_gui_item*>(parent);
		if(parent_gui_item != nullptr)
		{
			const qreal parent_right = parent->x() + parent->width();
			if(::fabs(my_right-parent_right) < 3. && ::fabs(my_top-parent->y()) < 3.) // Overlap with a parent hot corner
			{
				m_overlapping_parents.push_back(parent);
				parent_gui_item->set_on_top(false);
			}
		}
		parent = parent->parentItem();
	}

	qreal z = m_overlapping_parents.size();
	for(auto overlapped_parent : m_overlapping_parents)
	{
		overlapped_parent->setZ(z);
		z -= 1;
	}

	emit overlapping_parents_changed(overlapping_parents());
}

void dynamic_gui_item::clear()
{
	k3d::log() << debug << "clearing item " << qmlContext(this)->nameForObject(this).toStdString() << std::endl;
	for_each_child([](igui_element& elem)
	{
		auto item = dynamic_cast<QQuickItem*>(&elem);
		return_if_fail(item != nullptr);
		auto dyn_item = dynamic_cast<dynamic_gui_item*>(item);
		item->setParentItem(nullptr);
		item->setParent(nullptr);
		item->deleteLater();
	});
	clear_children();
	detail::recursively_rebuild_overlapping_parents(this);
}

void dynamic_gui_item::itemChange(ItemChange change, const ItemChangeData& value)
{
	if(change == QQuickItem::ItemParentHasChanged && value.item == nullptr)
	{
		k3d::log() << "parent set to null, disabling decorations" << std::endl;
		m_decoration->setVisible(false);
		m_decoration->setEnabled(false);
		m_decoration->deleteLater();
	}
	if(change == QQuickItem::ItemChildAddedChange || change == QQuickItem::ItemChildRemovedChange)
	{
		detail::recursively_rebuild_overlapping_parents(value.item);
	}

	QQuickItem::itemChange(change, value);
}

void dynamic_gui_item::geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry)
{
	QQuickItem::geometryChanged(newGeometry, oldGeometry);
	detail::recursively_rebuild_overlapping_parents(this);
}

} // namespace qtui

} // namespace k3d

