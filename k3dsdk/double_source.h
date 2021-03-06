#ifndef K3DSDK_DOUBLE_SOURCE_H
#define K3DSDK_DOUBLE_SOURCE_H

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

#include <k3dsdk/data.h>
#include <k3dsdk/idouble_source.h>
#include <k3dsdk/value_demand_storage.h>

namespace k3d
{

template<typename derived_t>
class double_source :
	public idouble_source
{
public:
	iproperty& double_source_output()
	{
		return m_output_double;
	}

	hint::slot_t make_update_double_slot()
	{
		return m_output_double.make_slot();
	}

protected:
	double_source() :
		m_output_double(
			init_owner(*static_cast<derived_t*>(this))
			+ init_name("output_double")
			+ init_label(_("Output Double"))
			+ init_description("Output double")
			+ init_value(0.0))
	{
		m_output_double.set_update_slot(boost::bind(&double_source<derived_t>::execute, this));
	}

private:
	k3d_data(double_t, data::immutable_name, data::change_signal, data::no_undo, data::value_demand_storage, data::no_constraint, data::read_only_property, data::no_serialization) m_output_double;

	/// Called whenever the output has been modified and needs to be updated.
	void execute(const std::vector<ihint*>& Hints, double_t& Output)
	{
		// We can safely ignore any hints ...
		on_update_double(Output);
	}

	virtual void on_update_double(double_t& Output) = 0;
};

} // namespace k3d

#endif // !K3DSDK_DOUBLE_SOURCE_H

