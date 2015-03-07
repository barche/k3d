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
#include <k3dsdk/render_state_gl.h>
#include <k3dsdk/utility_gl.h>

namespace k3d
{

class icamera;

namespace gl
{

	render_state::render_state(icamera &Camera, const unsigned long PixelWidth, const unsigned long PixelHeight) :
		camera(Camera),
		draw_two_sided(true)
	{
		glGetIntegerv(GL_VIEWPORT, static_cast<GLint*>(gl_viewport));

		k3d::rectangle window_rect(0, 0, 0, 0);
		k3d::rectangle camera_rect(0, 0, 0, 0);
		double near = 0;
		double far = 0;
		bool orthographic = false;
		k3d::gl::calculate_projection(Camera, PixelWidth, PixelHeight, window_rect, camera_rect, near, far, orthographic);

		gl_projection_matrix = k3d::gl::projection(orthographic, window_rect, near, far);

		// Setup projection ...
		if(orthographic)
		{
			const k3d::matrix4 transform_matrix = k3d::property::pipeline_value<k3d::matrix4>(Camera.transformation().matrix_source_output());
			const k3d::point3 world_position = transform_matrix * k3d::point3(0, 0, 0);
			const k3d::point3 world_target = boost::any_cast<k3d::point3>(Camera.world_target().property_internal_value());
			const double distance = k3d::distance(world_position, world_target);

			const double window_aspect = (window_rect.x2 - window_rect.x1) / (window_rect.y1 - window_rect.y2);
			const double window_tan_fov = (window_rect.y1 - window_rect.y2) * 0.5 / near;
			const double window_size = distance * window_tan_fov;

			orthographic = true;

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

		const k3d::matrix4 transform_matrix = k3d::property::pipeline_value<k3d::matrix4>(Camera.transformation().matrix_source_output());

		// Invert the rotation
		k3d::angle_axis orientation(k3d::euler_angles(transform_matrix, k3d::euler_angles::ZXYstatic));
		orientation.angle = -orientation.angle;
		k3d::vector3 translation = -k3d::extract_translation(transform_matrix);

		const k3d::matrix4 view_matrix =  k3d::scale3(1., 1., -1.)*k3d::rotate3(orientation)*k3d::translate3(translation);
		gl_projection_view_matrix = gl_projection_matrix * view_matrix;

		// Note: default expected matrices:
		// projection matrix: 1.5 0 0 0 0 1.9040660858154297 0 0 0 0 -1.0020020008087158 -2.0020020008087158 0 0 -1 0
		// view matrix: 0.79998916387557983 0.59999555349349976 -1.2516975402832031e-06 -6.0915946960449219e-05 0.2228316068649292 -0.29711368680000305 0.92847216129302979 2.6702880859375e-05 -0.55708229541778564 0.74277853965759277 0.37139114737510681 -26.925716400146484 0 0 0 1
	}

} // namespace gl

} // namespace k3d


