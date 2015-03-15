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

#include <k3d-platform-config.h>
#include <k3dsdk/algebra.h>
#include <k3dsdk/bounding_box3.h>
#include <k3dsdk/icamera.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imaterial.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/inode_collection.h>
#include <k3dsdk/iprojection.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/material.h>
#include <k3dsdk/plane.h>
#include <k3dsdk/rectangle.h>
#include <k3dsdk/utility_gl.h>

#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

#include <algorithm>
#include <set>

namespace k3d
{

namespace gl
{

namespace detail
{

class redraw
{
public:
	redraw(const irender_viewport::redraw_type_t RedrawType) :
		m_redraw_type(RedrawType)
	{
	}

	void operator()(inode* const Object)
	{
		if(irender_viewport* const render_engine = dynamic_cast<irender_viewport*>(Object))
			render_engine->redraw_request_signal()(m_redraw_type);
	}

protected:
	const irender_viewport::redraw_type_t m_redraw_type;
};

} // namespace detail

void draw(const plane& Plane)
{
	const k3d::vector3 n = Plane.normal;

	k3d::vector3 nx(-0.7071, -0.7071, 0);
	k3d::vector3 ny(0.7071, -0.7071, 0);
	const k3d::vector3 x_projection = n ^ vector3(n[1], n[0], n[2]);
	if(x_projection.length())
	{
		nx = normalize(n ^ vector3(n[1], n[0], n[2]));
		ny = normalize(n ^ nx);
	}

	const k3d::point3 origin = k3d::point3(0, 0, 0) + (Plane.normal * Plane.distance);

	glBegin(GL_LINE_LOOP);
		vertex3d(origin - nx + ny);
		vertex3d(origin + nx + ny);
		vertex3d(origin + nx - ny);
		vertex3d(origin - nx - ny);
	glEnd();

	glBegin(GL_LINES);
		vertex3d(origin);
		vertex3d(origin + n);
	glEnd();
}

void draw(const bounding_box3& Box)
{
	glBegin(GL_QUADS);
		glNormal3d(0, 0, -1);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.nz);

		glNormal3d(0, 0, 1);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.pz);

		glNormal3d(0, 1, 0);
		glVertex3d(Box.nx, Box.py, Box.pz);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.nx, Box.py, Box.nz);

		glNormal3d(0, -1, 0);
		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.pz);

		glNormal3d(-1, 0, 0);
		glVertex3d(Box.nx, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.ny, Box.pz);

		glNormal3d(1, 0, 0);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.ny, Box.nz);
	glEnd();
}

void draw_bounding_box(const bounding_box3& Box)
{
	glBegin(GL_LINE_LOOP);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.px, Box.ny, Box.pz);
		glVertex3d(Box.nx, Box.ny, Box.pz);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);
		glVertex3d(Box.px, Box.py, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.pz);
	glEnd();

	glBegin(GL_LINES);
		glVertex3d(Box.nx, Box.ny, Box.nz);
		glVertex3d(Box.nx, Box.py, Box.nz);

		glVertex3d(Box.px, Box.ny, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);

		glVertex3d(Box.px, Box.ny, Box.pz);
		glVertex3d(Box.px, Box.py, Box.pz);

		glVertex3d(Box.nx, Box.ny, Box.pz);
		glVertex3d(Box.nx, Box.py, Box.pz);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glVertex3d(Box.nx, Box.py, Box.nz);
		glVertex3d(0.5 * (Box.nx + Box.px), Box.ny, Box.nz);
		glVertex3d(Box.px, Box.py, Box.nz);
	glEnd();
}

void redraw_all(idocument& Document, const irender_viewport::redraw_type_t RedrawType)
{
	std::for_each(Document.nodes().collection().begin(), Document.nodes().collection().end(), detail::redraw(RedrawType));
}

void tex_image_2d(const bitmap& Source)
{
	// Compute power-of-two dimensions ...
	unsigned long width = Source.width();
	unsigned long height = Source.height();

	if(width & (width - 1))
	{
		width |= (width >> 1);
		width |= (width >> 2);
		width |= (width >> 4);
		width |= (width >> 8);
		width |= (width >> 16);
		width += 1;
		width /= 2;
	}

	if(height & (height - 1))
	{
		height |= (height >> 1);
		height |= (height >> 2);
		height |= (height >> 4);
		height |= (height >> 8);
		height |= (height >> 16);
		height += 1;
		height /= 2;
	}

	boost::gil::rgba8_image_t target(0, 0);
	assert_not_implemented();
/*
	if(extension::query("GL_ARB_texture_non_power_of_two") || (width == Source.width() && height == Source.height()))
	{
		target.recreate(Source.width(), Source.height());
		boost::gil::copy_and_convert_pixels(boost::gil::const_view(Source), boost::gil::view(target));
	}
	else
*/
	{
		target.recreate(width, height);
#ifdef K3D_API_DARWIN
    assert_not_implemented();
#else
		boost::gil::resize_view(boost::gil::color_converted_view<boost::gil::rgba8_pixel_t>(boost::gil::const_view(Source)), boost::gil::view(target), boost::gil::bilinear_sampler());
#endif
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		target.width(),
		target.height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		&boost::gil::view(target)[0]);
}

const point3 project(const point3& WorldCoords)
{
	k3d::point3 result;
	
	GLdouble modelview[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	GLint error = gluProject(
		WorldCoords[0],
		WorldCoords[1],
		WorldCoords[2],
		modelview,
		projection,
		viewport,
		&result[0],
		&result[1],
		&result[2]);
	
	return_val_if_fail(error, result);

	return result;
}

k3d::rectangle window_rectangle(icamera& Camera, const uint_t PixelWidth, const uint_t PixelHeight)
{
	rectangle result;

	const rectangle camera_rect = camera_rectangle(Camera);
	return_val_if_fail(camera_rect.x1 != camera_rect.x2 && camera_rect.y1 != camera_rect.y2, result);

	const double frustum_ratio = (camera_rect.x2 - camera_rect.x1) / (camera_rect.y1 - camera_rect.y2);
	const double raster_ratio = static_cast<double>(PixelWidth) / static_cast<double>(PixelHeight);

	if(raster_ratio > frustum_ratio)
	{
		const double width = 0.5 * raster_ratio * camera_rect.height();

		result.x1 = ((camera_rect.x1 + camera_rect.x2) * 0.5) - width;
		result.x2 = ((camera_rect.x1 + camera_rect.x2) * 0.5) + width;
		result.y1 = camera_rect.y1;
		result.y2 = camera_rect.y2;
	}
	else
	{
		const double height = 0.5 * camera_rect.width() / raster_ratio;

		result.x1 = camera_rect.x1;
		result.x2 = camera_rect.x2;
		result.y1 = ((camera_rect.y1 + camera_rect.y2) * 0.5) + height;
		result.y2 = ((camera_rect.y1 + camera_rect.y2) * 0.5) - height;
	}
}

k3d::rectangle camera_rectangle(icamera& Camera)
{
	rectangle result;

	iprojection* projection = k3d::property::pipeline_value<iprojection*>(Camera.projection());
	return_val_if_fail(projection != nullptr, result);

	result.x1 = k3d::property::pipeline_value<double>(projection->left());
	result.x2 = k3d::property::pipeline_value<double>(projection->right());
	if(result.x2 < result.x1)
		std::swap(result.x1, result.x2);

	result.y1 = k3d::property::pipeline_value<double>(projection->top());
	result.y2 = k3d::property::pipeline_value<double>(projection->bottom());
	if(result.y1 < result.y2)
		std::swap(result.y1, result.y2);
}


} // namespace gl

} // namespace k3d

