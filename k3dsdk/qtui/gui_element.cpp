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

#include <k3dsdk/result.h>

#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/gui_element.h>
#include <k3dsdk/qtui/utility.h>

#include <QQmlProperty>

namespace k3d
{

namespace qtui
{

void gui_element::for_each_child(std::function<void(igui_element&)> Functor) const
{
	for(igui_element* elem : m_children)
	{
		Functor(*elem);
	}
}

void gui_element::for_each_attribute(std::function<void(const k3d::string_t& Name, const boost::any& value)> Functor)
{
	for(const k3d::string_t& name : m_attributes)
	{
		QQmlContext* ctx = qmlContext(&qt_object());
		if(ctx == nullptr)
		{
			k3d::log() << error << "Qml context is null, can't resolve property " << name << std::endl;
			continue;
		}
		QQmlProperty prop(&qt_object(), k3d::convert<QString>(name), ctx);
		if(!prop.isValid())
		{
			k3d::log() << debug << "Skipping invalid property: " << name << std::endl;
			continue;
		}
		Functor(name, prop);
	}
}

k3d::string_t gui_element::name() const
{
	return qml_type_name(*qt_object().metaObject());
}

void gui_element::register_child(igui_element* Element)
{
	m_children.push_back(Element);
}

void gui_element::clear_children()
{
	m_children.clear();
}

void gui_element::add_attribute(const k3d::string_t& Name)
{
	m_attributes.push_back(Name);
}

const QObject& gui_element::qt_object() const
{
	const QObject* q_obj = dynamic_cast<const QObject*>(this);
	assert_error(q_obj != nullptr);
	return *q_obj;
}

QObject& gui_element::qt_object()
{
	QObject* q_obj = dynamic_cast<QObject*>(this);
	assert_error(q_obj != nullptr);
	return *q_obj;
}

gui_element::gui_element()
{
}

} // namespace qtui

} // namespace k3d

