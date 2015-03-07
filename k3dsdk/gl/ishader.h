#ifndef K3DSDK_GL_ISHADER_H
#define K3DSDK_GL_ISHADER_H

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

#include <k3dsdk/gl.h>
#include <k3dsdk/ihint.h>
#include <k3dsdk/iunknown.h>
#include <k3dsdk/signal_system.h>

namespace k3d
{

namespace gl
{

/// Abstract interface for OpenGL shaders
class ishader :
	public virtual iunknown
{
public:
	/// Type of the changed signal when the shader has changed
	typedef boost::signals2::signal<void(ihint*)> changed_signal_t;

	/// Return the current ID of the compiled shader, or 0 if something is wrong
	virtual GLuint shader_id() = 0;

	/// Request deletion of the shader, if it is no longer used
	virtual void request_delete() = 0;

	/// Connect to the changed signal that indicates the shader ID has changed
	virtual boost::signals2::connection connect_shader_changed_signal(const changed_signal_t::slot_type& Slot) = 0;

	/// Return the shader type
	virtual GLenum type() const = 0;

protected:
	ishader() {}
	ishader(const ishader& Other) : iunknown(Other) {}
	ishader& operator=(const ishader&) { return *this; }
	virtual ~ishader() {}
};

} // namespace gl

} // namespace k3d

#endif // !K3DSDK_GL_ISHADER_H

