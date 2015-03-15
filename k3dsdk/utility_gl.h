#ifndef K3DSDK_UTILITY_GL_H
#define K3DSDK_UTILITY_GL_H

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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/bitmap.h>
#include <k3dsdk/color.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/property.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/rectangle.h>

namespace k3d
{

class bounding_box3;
class icamera;
class idocument;
class plane;

namespace gl
{

/// Converts an OpenGL matrix into a standard K-3D matrix
inline const matrix4 matrix(GLdouble* GLMatrix)
{
	matrix4 result;
	memcpy(&result[0][0], &GLMatrix[0], 16 * sizeof(GLdouble));
	return transpose(result);
}

/// Encapsulate OpenGL vector data
template<int Dim, typename FloatT=GLfloat>
class gl_points
{
public:
	void reserve(const k3d::uint_t Size)
	{
		m_points.reserve(Size*Dim);
	}

	// Size in number of points
	k3d::uint_t size() const
	{
		return m_points.size() / Dim;
	}

	// Size in bytes
	k3d::uint_t byte_size() const
	{
		return sizeof(FloatT) * m_points.size();
	}

	template<typename... ComponentsT>
	void push_back(const ComponentsT... Components)
	{
		static_assert(sizeof...(Components) == Dim, "Invalid dimension");
		push_back_impl(Components...);
	}

	const FloatT* data()
	{
		return m_points.data();
	}

private:
	// Ends recursion
	template<typename NumberT>
	void push_back_impl(const NumberT First)
	{
		m_points.push_back(static_cast<FloatT>(First));
	}

	// Recurse over arguments
	template<typename NumberT, typename... ComponentsT>
	void push_back_impl(const NumberT First, const ComponentsT... Others)
	{
		push_back_impl(First);
		push_back_impl(Others...);
	}


	std::vector<FloatT> m_points;
};

/// Converts an OpenGL matrix into a standard K-3D matrix
inline const matrix4 matrix(const GLfloat GLMatrix[16])
{
	matrix4 result;
	for(uint_t i = 0; i != 4; ++i)
	{
		for(uint_t j = 0; j != 4; ++j)
		{
			result[i][j] = static_cast<double_t>(GLMatrix[j*4+i]);
		}
	}
	return result;
}

/// Pushes a matrix onto the OpenGL matrix stack
inline void push_matrix(const matrix4& Matrix)
{
	double glmatrix[16];
	transpose(Matrix).CopyArray(glmatrix);
	glMultMatrixd(glmatrix);
}

/// Passes a k3d::point4 to glVertex4d()
inline void vertex4d(const point4& Vertex)
{
	glVertex4d(Vertex.n[0], Vertex.n[1], Vertex.n[2], Vertex.n[3]);
}

/// Passes a k3d::point3 to glVertex3d()
inline void vertex3d(const point3& Vertex)
{
	glVertex3d(Vertex.n[0], Vertex.n[1], Vertex.n[2]);
}

/// Passes a k3d::vector3 to glNormal3d()
inline void normal3d(const vector3& Vector)
{
	glNormal3d(Vector.n[0], Vector.n[1], Vector.n[2]);
}

/// Passes a k3d::normal3 to glNormal3d()
inline void normal3d(const normal3& Vector)
{
	glNormal3d(Vector.n[0], Vector.n[1], Vector.n[2]);
}

/// Passes a k3d::color to glColor3d()
inline void color3d(const color& Color)
{
	glColor3d(Color.red, Color.green, Color.blue);
}

/// Passes a k3d::color to glMaterialfv()
inline void material(GLenum Face, GLenum PName, const color& Color, double Alpha = 1.0)
{
	GLfloat color[] = { static_cast<GLfloat>(Color.red), static_cast<GLfloat>(Color.green), static_cast<GLfloat>(Color.blue), static_cast<GLfloat>(Alpha) };
	glMaterialfv(Face, PName, color);
}

/// Sets an OpenGL state flag
inline void set(GLenum Flag, const bool State)
{
	if(State)
		glEnable(Flag);
	else
		glDisable(Flag);
}

/// Provides exception- and return-safe RAII behavior for saving / restoring OpenGL attributes
struct store_attributes
{
	store_attributes(const GLbitfield Mask = GL_ALL_ATTRIB_BITS)
	{
//		glPushAttrib(Mask); //deprecated
	}

	~store_attributes()
	{
//		glPopAttrib(); //deprecated
	}
};

/// Convenience function for refreshing all OpenGL render engines
void redraw_all(idocument& Document, const irender_viewport::redraw_type_t RedrawType);
/// Draws a 1x1 plane
void draw(const plane& Plane);
/// Draws a box
void draw(const bounding_box3& Box);
/// Draws a standard bounding box to make it easier to visualize an object
void draw_bounding_box(const bounding_box3& Box);
/// Passes a k3d::bitmap to glTexImage2D(), handling non-power-of-two sizes and translations between image formats
void tex_image_2d(const bitmap& Bitmap);

/// Projects a point in world coordinates into screen space, returning the 2D widget coordinates and Z-buffer depth
const point3 project(const point3& WorldCoords);

/// Calculate the window rectangle
k3d::rectangle window_rectangle(k3d::icamera& Camera, const uint_t PixelWidth, const uint_t PixelHeight);

/// Calculate the camera rectangle
k3d::rectangle camera_rectangle(icamera& Camera);

/// Return the OpenGL projection matrix
inline matrix4 projection(const bool Orthographic, const rectangle& WindowRect, const k3d::double_t Near, const k3d::double_t Far)
{
	if(Orthographic)
		return matrix4(vector4(2./(WindowRect.x2-WindowRect.x1), 0., 0., (WindowRect.x1 + WindowRect.x2) / (WindowRect.x1 - WindowRect.x2)),
                 vector4(0., 2./(WindowRect.y1-WindowRect.y2), 0., (WindowRect.y2 + WindowRect.y1) / (WindowRect.y2 - WindowRect.y1)),
								 vector4(0., 0., 2./(Near-Far), (Near+Far)/(Near-Far)),
								 vector4(0., 0., 0., 1.));

	// Perspective projection
	return matrix4(vector4(2.*Near/(WindowRect.x2-WindowRect.x1), 0., (WindowRect.x1 + WindowRect.x2) / (WindowRect.x2 - WindowRect.x1), 0.),
                 vector4(0., 2.*Near/(WindowRect.y1-WindowRect.y2), (WindowRect.y2 + WindowRect.y1) / (WindowRect.y1 - WindowRect.y2), 0),
								 vector4(0., 0., (Near+Far)/(Near-Far), 2.*Near*Far/(Near-Far)),
								 vector4(0., 0., -1., 0.));
}

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_UTILITY_GL_H

