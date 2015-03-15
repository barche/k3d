#ifndef K3DSDK_VIEWPORT_STATE_H
#define K3DSDK_VIEWPORT_STATE_H

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
		\brief Declares viewport_state, a boilerplate implementation of iviewport_state
		\author Bart Janssens
*/

#include <k3d-i18n-config.h>

#include <k3dsdk/algebra.h>
#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/iviewport_state.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node_storage.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/value_demand_storage.h>
#include <k3dsdk/utility_gl.h>


namespace module
{

namespace core
{

/// Abstract interface for objects that can consume time data
class viewport_state : public k3d::node, public k3d::iviewport_state
{
public:
	viewport_state(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		k3d::node(Factory, Document),
		m_camera(init_owner(*this) + init_name("camera") + init_label(_("Camera")) + init_description(_("Camera that renders the scene")) + init_value(static_cast<k3d::icamera*>(nullptr))),
		m_render_engine(init_owner(*this) + init_name("render_engine") + init_label(_("Render Engine")) + init_description(_("Render engine for this viewport")) + init_value(static_cast<k3d::gl::irender_viewport*>(nullptr))),
		m_pixel_width(init_owner(*this) + init_name("pixel_width") + init_label(_("Pixel Width")) + init_description(_("Width of the viewport, in pixels")) + init_value(0)),
		m_pixel_height(init_owner(*this) + init_name("pixel_height") + init_label(_("Pixel Height")) + init_description(_("Height of the viewport, in pixels")) + init_value(0)),
		m_projection_matrix(init_owner(*this) + init_name("projection_matrix") + init_label(_("Projection Matrix")) + init_description(_("Projection matrix, as computed from the camera settings and the viewport dimensions")) + init_value(k3d::identity3()))
		{
			m_camera.property_changed_signal().connect(boost::bind(&viewport_state::on_camera_changed, this, _1));
			m_pixel_width.property_changed_signal().connect(m_projection_matrix.make_slot());
			m_pixel_height.property_changed_signal().connect(m_projection_matrix.make_slot());
			m_projection_matrix.set_update_slot(boost::bind(&viewport_state::update_projection_matrix, this, _1, _2));
		}


	/// The camera used by this viewport
	virtual k3d::iproperty& camera_property()
	{
		return m_camera;
	}

	/// The OpenGL render engine used by this viewport
	virtual k3d::iproperty& render_engine_property()
	{
		return m_render_engine;
	}

	/// Width (in pixels) of the viewport
	virtual k3d::iproperty& pixel_width_property()
	{
		return m_pixel_width;
	}

	/// Height (in pixels) of the viewport
	virtual k3d::iproperty& pixel_height_property()
	{
		return m_pixel_height;
	}

	/// Projection matrix related to the current viewport
	virtual k3d::iproperty& projection_matrix_property()
	{
		return m_projection_matrix;
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<viewport_state,
			k3d::interface_list<k3d::iviewport_state> >factory(
			k3d::uuid("68baef8a-b225-498d-9a10-eca314a77c4b"),
			"ViewportState",
			_("State related to a viewport"),
			"RenderEngine",
			k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	void on_camera_changed(k3d::ihint*)
	{
		m_projection_connection.disconnect();
		k3d::icamera* camera = m_camera.pipeline_value();
		if(camera != nullptr)
		{
			m_projection_connection = camera->projection().property_changed_signal().connect(m_projection_matrix.make_slot());
		}
		m_projection_matrix.update();
	}

	void update_projection_matrix(const std::vector<k3d::ihint*>&, k3d::matrix4& Output)
	{
		Output = k3d::identity3();
		k3d::icamera* camera = m_camera.pipeline_value();
		if(camera == nullptr)
		{
			return;
		}

		k3d::iprojection* projection = k3d::property::pipeline_value<k3d::iprojection*>(camera->projection());
		return_if_fail(projection != nullptr);
		const bool orthographic = (dynamic_cast<k3d::iorthographic*>(projection) != nullptr);
		Output = k3d::gl::projection(orthographic, k3d::gl::window_rectangle(*camera, m_pixel_width.pipeline_value(), m_pixel_height.pipeline_value()), k3d::property::pipeline_value<k3d::double_t>(projection->near()), k3d::property::pipeline_value<k3d::double_t>(projection->far()));
	}

	k3d_data(k3d::icamera*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_camera;
	k3d_data(k3d::gl::irender_viewport*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_render_engine;
	k3d_data(k3d::uint_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_pixel_width;
	k3d_data(k3d::uint_t, immutable_name, change_signal, no_undo, local_storage, no_constraint, writable_property, no_serialization) m_pixel_height;
	k3d_data(k3d::matrix4, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_projection_matrix;

	boost::signals2::connection m_projection_connection;
};

k3d::iplugin_factory& viewport_state_factory()
{
	return viewport_state::get_factory();
}

} // namespace core

} // namespace module

#endif // !K3DSDK_VIEWPORT_STATE_H

