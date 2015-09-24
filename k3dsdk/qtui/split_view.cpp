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
#include <k3dsdk/result.h>

#include <k3dsdk/qtui/split_view.h>
#include <k3dsdk/qtui/utility.h>

#include <deque>

k3d::qtui::qml_type_registrator<k3d::qtui::split_view> register_split_view(QUrl(), true);

namespace k3d
{

namespace qtui
{

split_view::split_view(QQuickItem *parent) : QQuickItem(parent)
{
	add_attribute("orientation");
}

split_view::~split_view()
{
}

void split_view::set_orientation(const Qt::Orientation Orientation)
{
	m_orientation = Orientation;
	emit orientation_changed(m_orientation);
}

void split_view::componentComplete()
{
	QQuickItem::componentComplete();

	QQuickItem* qt_split_view = load_qml(QUrl(QStringLiteral("qrc:/qml/detail/split_view.qml")), this);
	return_if_fail(qt_split_view != nullptr);
	std::deque<gui_element*> registered_elements;
	for(QQuickItem* item : childItems())
	{
		if(item == qt_split_view)
			continue;

		gui_element* element = dynamic_cast<gui_element*>(item);
		if(element == nullptr)
		{
			k3d::log() << warning << "Item of type " << item->metaObject()->className() << " will not be saved in config since it is not a k3d::igui_element" << std::endl;
		}
		else
		{
			register_child(element);
			registered_elements.push_back(element);
		}

		QMetaObject::invokeMethod(qt_split_view, "addItem", Q_ARG(QVariant, QVariant::fromValue(item)));
	}

	// Register a width or height attribute (depending on orientation) on each element except for the last
	registered_elements.pop_back();
	const k3d::string_t attrib_name = m_orientation == Qt::Horizontal ? "width" : "height";
	for(gui_element* el : registered_elements)
	{
		el->add_attribute(attrib_name);
	}
}

} // namespace qtui

} // namespace k3d

