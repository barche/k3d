#ifndef K3DSDK_GL_IPROGRAM_H
#define K3DSDK_GL_IPROGRAM_H

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

#include <k3dsdk/color.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/ihint.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace gl
{

/// Abstract interface for OpenGL programs
class iprogram :
	public virtual iunknown
{
public:
	/// Bind the program for rendering (as in glBindProgramPipeline​). Returns true on success
	virtual bool bind() = 0;

	/// Convenience functions to set uniforms froim K-3D types
	virtual void set_uniform(const std::string& Name, const k3d::matrix4& Matrix) = 0;
	virtual void set_uniform(const std::string& Name, const k3d::color& Color) = 0;

protected:
	iprogram() {}
	iprogram(const iprogram& Other) : iunknown(Other) {}
	iprogram& operator=(const iprogram&) { return *this; }
	virtual ~iprogram() {}
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_GL_IPROGRAM_H

