// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/icamera.h>
#include <k3dsdk/imatrix_source.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/iviewport_state.h>
#include <k3dsdk/property.h>
#include <k3dsdk/render_state_gl.h>
#include <k3dsdk/utility_gl.h>

namespace k3d
{

namespace gl
{

	render_state::render_state(iviewport_state& ViewportState) :
		camera(dynamic_cast<k3d::icamera*>(k3d::property::pipeline_value<k3d::inode*>((ViewportState.camera_property())))),
		draw_two_sided(true)
	{
		const k3d::uint_t pixel_width = k3d::property::pipeline_value<k3d::uint_t>(ViewportState.pixel_width_property());
		const k3d::uint_t pixel_height = k3d::property::pipeline_value<k3d::uint_t>(ViewportState.pixel_height_property());
		assert(camera != nullptr);

		k3d::iprojection* projection = k3d::property::pipeline_value<k3d::iprojection*>(camera->projection());
		assert(projection != nullptr);

		const double near = k3d::property::pipeline_value<double>(projection->near());
		const double far = k3d::property::pipeline_value<double>(projection->far());
		orthographic = (dynamic_cast<k3d::iorthographic*>(projection) != nullptr);

		const k3d::rectangle window_rect = k3d::gl::window_rectangle(*camera, pixel_width, pixel_height);
		const k3d::rectangle camera_rect = k3d::gl::camera_rectangle(*camera);

		gl_projection_matrix = k3d::property::pipeline_value<k3d::matrix4>(ViewportState.projection_matrix_property());

		// Setup projection ...
		if(orthographic)
		{
			const k3d::matrix4 transform_matrix = k3d::property::pipeline_value<k3d::matrix4>(camera->transformation().matrix_source_output());
			const k3d::point3 world_position = transform_matrix * k3d::point3(0, 0, 0);
			const k3d::point3 world_target = boost::any_cast<k3d::point3>(camera->world_target().property_internal_value());
			const double distance = k3d::distance(world_position, world_target);

			const double window_aspect = (window_rect.x2 - window_rect.x1) / (window_rect.y1 - window_rect.y2);
			const double window_tan_fov = (window_rect.y1 - window_rect.y2) * 0.5 / near;
			const double window_size = distance * window_tan_fov;

			gl_window_frustum_left = -window_size * window_aspect;
			gl_window_frustum_right = window_size * window_aspect;
			gl_window_frustum_top = window_size;
			gl_window_frustum_bottom = -window_size;
			gl_window_frustum_near = near;
			gl_window_frustum_far = far;

			const double camera_aspect = (camera_rect.x2 - camera_rect.x1) / (camera_rect.y1 - camera_rect.y2);

			if(camera_aspect > window_aspect)
			{
				gl_camera_frustum_left = gl_window_frustum_left;
				gl_camera_frustum_right = gl_window_frustum_right;
				gl_camera_frustum_top = gl_window_frustum_top * window_aspect / camera_aspect;
				gl_camera_frustum_bottom = gl_window_frustum_bottom * window_aspect / camera_aspect;
				gl_camera_frustum_near = near;
				gl_camera_frustum_far = far;
			}
			else
			{
				gl_camera_frustum_left = gl_window_frustum_left * camera_aspect / window_aspect;
				gl_camera_frustum_right = gl_window_frustum_right * camera_aspect / window_aspect;
				gl_camera_frustum_top = gl_window_frustum_top;
				gl_camera_frustum_bottom = gl_window_frustum_bottom;
				gl_camera_frustum_near = near;
				gl_camera_frustum_far = far;
			}
		}
		else
		{
			orthographic = false;

			gl_window_frustum_left = window_rect.x1;
			gl_window_frustum_right = window_rect.x2;
			gl_window_frustum_top = window_rect.y1;
			gl_window_frustum_bottom = window_rect.y2;
			gl_window_frustum_near = near;
			gl_window_frustum_far = far;

			gl_camera_frustum_left = camera_rect.x1;
			gl_camera_frustum_right = camera_rect.x2;
			gl_camera_frustum_top = camera_rect.y1;
			gl_camera_frustum_bottom = camera_rect.y2;
			gl_camera_frustum_near = near;
			gl_camera_frustum_far = far;
		}

		const k3d::matrix4 view_matrix = k3d::property::pipeline_value<k3d::matrix4>(camera->view_matrix());
		gl_projection_view_matrix = gl_projection_matrix * view_matrix;

//		k3d::log() << debug << "projection matrix: " << gl_projection_matrix << std::endl;
//		k3d::log() << debug << "view matrix: " << view_matrix << std::endl;

		// Note: default expected matrices:
		// projection matrix: 1.5 0 0 0 0 1.9040660858154297 0 0 0 0 -1.0020020008087158 -2.0020020008087158 0 0 -1 0
		// view matrix: 0.79998916387557983 0.59999555349349976 -1.2516975402832031e-06 -6.0915946960449219e-05 0.2228316068649292 -0.29711368680000305 0.92847216129302979 2.6702880859375e-05 -0.55708229541778564 0.74277853965759277 0.37139114737510681 -26.925716400146484 0 0 0 1
	}

} // namespace gl

} // namespace k3d


