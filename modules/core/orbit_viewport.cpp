#ifndef K3DSDK_orbit_viewport_H
#define K3DSDK_orbit_viewport_H

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
		\brief Declares orbit_viewport, a boilerplate implementation of iorbit_viewport
		\author Bart Janssens
*/

#include <k3d-i18n-config.h>

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/imatrix_source.h>
#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/property.h>
#include <k3dsdk/transform.h>
#include <k3dsdk/viewport_tool.h>


namespace module
{

namespace core
{

class orbit_viewport : public k3d::viewport_tool
{
public:
	orbit_viewport(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		k3d::viewport_tool(Factory, Document),
		m_dragging(false)
		{
		}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<orbit_viewport,
			k3d::interface_list<k3d::iviewport_tool> >factory(
			k3d::uuid("D077DCA0-BFFB-421E-98DE-99577FFC887C"),
			"OrbitViewport",
			_("Viewport camera orbit (i.e. rotate around origin) tool"),
			"Viewport",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void process_input_event(const k3d::input_event& Event)
	{
		const k3d::point2 current_position = Event.position;

		if(Event.type == k3d::input_type::BUTTON_PRESSED && Event.button == k3d::input_button::MOUSE_RIGHT)
		{
			m_last_position = current_position;
			m_dragging = true;
			return;
		}

		if(Event.type == k3d::input_type::POSITION_CHANGE && Event.button == k3d::input_button::NONE)
		{
			if(!m_dragging)
				return;

			k3d::iviewport_state* state = m_viewport_state.internal_value();
			k3d::icamera* camera = dynamic_cast<k3d::icamera*>(k3d::property::internal_value<k3d::inode*>(state->camera_property()));
			return_if_fail(camera != nullptr);

			const k3d::matrix4 old_navigation_target = k3d::node_to_world_matrix(camera->navigation_target());//k3d::property::internal_value<k3d::matrix4>(camera->view_matrix());
			const k3d::vector3 look_vector = k3d::look_vector(old_navigation_target);
			const k3d::vector3 up_vector = k3d::up_vector(old_navigation_target);
			const k3d::vector3 right_vector = k3d::right_vector(old_navigation_target);
			const k3d::point3 position = k3d::position(old_navigation_target);
			const k3d::point3 target = k3d::property::internal_value<k3d::point3>(camera->world_target());

			const k3d::point2 current_ndc = state->normalized_device_coordinates(current_position);
			const k3d::point2 last_ndc = state->normalized_device_coordinates(m_last_position);

			const double direction = -1.0;
			const double sensitivity = k3d::pi();
			const double ysensitivity = -1.0;
			const double deltax = last_ndc[0] - current_ndc[0];
			const double deltay = last_ndc[1] - current_ndc[1];

			const k3d::vector3 up_axis = k3d::property::internal_value<k3d::vector3>(state->up_axis_property());

			const k3d::matrix4 matrix =
				k3d::rotate3(k3d::angle_axis(sensitivity * direction * deltax, up_axis)) *
				k3d::rotate3(k3d::angle_axis(ysensitivity * sensitivity * direction * deltay, right_vector));

			const k3d::vector3 new_look_vector = matrix * look_vector;
			const k3d::vector3 new_up_vector = matrix * up_vector;
			const k3d::point3 new_position = target + (matrix * (position - target));

			const k3d::matrix4 new_navigation_target = k3d::view_matrix(new_look_vector, new_up_vector, new_position);

			k3d::set_matrix(camera->navigation_target(), new_navigation_target);

			m_last_position = current_position;
			//wrap_mouse_pointer(Viewport);
		}

		if(Event.type == k3d::input_type::BUTTON_RELEASED && Event.button == k3d::input_button::MOUSE_RIGHT)
		{
			m_dragging = false;
			return;
		}
	}

	k3d::point2 m_last_position;
	bool m_dragging;
};

k3d::iplugin_factory& orbit_viewport_factory()
{
	return orbit_viewport::get_factory();
}

} // namespace core

} // namespace module

#endif // !K3DSDK_orbit_viewport_H

