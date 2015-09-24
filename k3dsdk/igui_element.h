#ifndef K3DSDK_IGUI_ELEMENT_H
#define K3DSDK_IGUI_ELEMENT_H

// K-3D
// Copyright (c) 1995-2015, K-3D
//
// Contact: bart@bartjanssens.org
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
		\brief Declares igui_element, an interface for K-3D GUI elements
		\author Bart Janssens
*/

#include <k3dsdk/iunknown.h>
#include <k3dsdk/types.h>

#include <boost/any.hpp>

#include <functional>

namespace k3d
{

/// Abstract interface for objects that are part of the GUI. This interface should allow serialization of user-customizable GUI elements to a config file,
/// by making it possible to traverse the tree of GUI elements and their attributes
class igui_element :
	public virtual iunknown
{
public:
	/// Loop over the child items
	virtual void for_each_child(std::function<void(igui_element&)> Functor) const = 0;

	/// Loop over the attributes, using a functor that takes the name and the value of the attribute
	virtual void for_each_attribute(std::function<void(const k3d::string_t&, const boost::any&)> Functor) = 0;

	virtual k3d::string_t name() const = 0;

protected:
	igui_element() {}
	igui_element(const igui_element&) {}
	igui_element& operator=(const igui_element&) { return *this; }
	virtual ~igui_element() {}
};

} // namespace k3d

#endif // !K3DSDK_IGUI_ELEMENT_H
