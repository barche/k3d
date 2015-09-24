#ifndef K3DSDK_QTUI_GUI_ELEMENT_H
#define K3DSDK_QTUI_GUI_ELEMENT_H

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

#include <k3dsdk/igui_element.h>

#include <QQuickItem>

#include <vector>

namespace k3d
{

namespace qtui
{

/// Name of the QMetaClassInfo meta-data that keeps the Qml type name
constexpr char qml_type_name_key[] = "qml_type_name";

/////////////////////////////////////////////////////////////////////////////
// gui_element

/// Base class for dynamically creatable (e.g. by user action) GUI items, with a boilerplate implementation of the igui_element interface
class gui_element :
	public k3d::igui_element
{
public:

	virtual void for_each_child(std::function<void(igui_element&)> Functor) const;

	/// Loop over attributes. Note that the boost::any contains always a string in this implementation
	virtual void for_each_attribute(std::function<void(const k3d::string_t& Name, const boost::any& value)> Functor);

	virtual k3d::string_t name() const;

	/// Register a pointer to a child. Lifetime management is done by Qt
	void register_child(igui_element* Element);

	/// Register a Qt property name to add to the list of attributes to traverse
	void add_attribute(const k3d::string_t& Name);

protected:
	/// Access to the concrete qt object type
	virtual const QObject& qt_object() const;
	virtual QObject& qt_object();

	/// Clear the list of children
	void clear_children();

	gui_element();

	gui_element(const gui_element&) {}
	gui_element& operator=(const gui_element&) { return *this; }
	virtual ~gui_element() {}

private:
	std::vector<igui_element*> m_children;
	std::vector<k3d::string_t> m_attributes;

};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_GUI_ELEMENT_H

