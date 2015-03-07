#ifndef K3DSDK_USER_NODE_SERIALIZATION_H
#define K3DSDK_USER_NODE_SERIALIZATION_H

// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/ipersistent.h>
#include <k3dsdk/iproperty.h>

namespace k3d
{

namespace property
{

/// Serialization policy for data containers that store a document node and can be serialized as XML
template<typename value_t, class property_policy_t>
class user_node_serialization :
public property_policy_t,
public ipersistent
{
	// If this assertion fires, it means that you're trying to use node_serialization with a non-node data type
	BOOST_STATIC_ASSERT((boost::is_base_and_derived<iunknown, typename boost::remove_pointer<value_t>::type>::value));

public:
	void save(xml::element& Element, const ipersistent::save_context& Context)
	{
		if(property_policy_t::internal_node())
		{
			Element.append(
			xml::element(
			"property",
			string_cast(Context.lookup.lookup_id(property_policy_t::internal_node())),
			xml::attribute("name", property_policy_t::name()),
			xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
			xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
			xml::attribute("type", type_string<value_t>()),
			xml::attribute("user_property", "generic")));
		}
		else
		{
			Element.append(
			xml::element(
			"property",
			"0",
			xml::attribute("name", property_policy_t::name()),
			xml::attribute("label", static_cast<iproperty&>(*this).property_label()),
			xml::attribute("description", static_cast<iproperty&>(*this).property_description()),
			xml::attribute("type", type_string<value_t>()),
			xml::attribute("user_property", "generic")));
		}
	}

	void load(xml::element& Element, const ipersistent::load_context& Context)
	{
		string_t value = Element.text;
		property_policy_t::set_value(dynamic_cast<value_t>(Context.lookup.lookup_object(from_string(value, static_cast<ipersistent_lookup::id_type>(0)))), 0);
	}

protected:
	template<typename init_t>
	user_node_serialization(const init_t& Init) :
	property_policy_t(Init)
	{
		Init.persistent_collection().enable_serialization(Init.name(), *this);
	}

private:
	ipersistent_lookup::id_type m_node_id;
};

} // namespace property

} // namespace k3d

#endif // !K3DSDK_USER_NODE_SERIALIZATION_H

