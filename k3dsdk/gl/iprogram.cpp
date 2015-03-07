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
	\author Bart Janssens
*/

#include <k3dsdk/data.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iproperty_collection.h>
#include <k3dsdk/ipersistent_collection.h>
#include <k3dsdk/node_storage.h>
#include <k3dsdk/property.h>
#include <k3dsdk/type_registry.h>
#include <k3dsdk/user_node_serialization.h>

#include <k3dsdk/gl/iprogram.h>

namespace k3d
{

namespace gl
{

class iprogram_property_factory : public k3d::property::iproperty_factory
{
public:
	iproperty* create(inode &Owner, const string_t &Name, const string_t &Label, const string_t &Description, const boost::any &Value)
	{
		k3d::iproperty_collection* property_collection = dynamic_cast<k3d::iproperty_collection*>(&Owner);
		k3d::ipersistent_collection* persistent_collection = dynamic_cast<k3d::ipersistent_collection*>(&Owner);

		return_val_if_fail(property_collection, nullptr);
		return_val_if_fail(persistent_collection, nullptr);

		iprogram* initial_value = boost::any_cast<iprogram*>(Value);

		typedef k3d::property::user_property<k3d_data(iprogram*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, property::user_node_serialization)> property_t;
		return new property_t(
			init_owner(Owner.document(), *property_collection, *persistent_collection, &Owner)
			+ init_name(Name.c_str())
			+ init_label(Label.c_str())
			+ init_description(Description.c_str())
			+ init_value(initial_value)
			);
	}
};

struct iprogram_registrator
{
	iprogram_registrator()
	{
		const string_t name = "k3d::gl::iprogram";
		k3d::register_type(typeid(k3d::gl::iprogram), name);
		k3d::register_type(typeid(k3d::gl::iprogram*), name + "*");
		k3d::property::register_property_factory(name + "*", std::shared_ptr<k3d::property::iproperty_factory>(new iprogram_property_factory()));
	}
};

iprogram_registrator register_iprogram;

} // namespace gl

} // namespace k3d


