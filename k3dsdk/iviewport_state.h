#ifndef K3DSDK_iviewport_state_H
#define K3DSDK_iviewport_state_H

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
		\brief Declares iviewport_state, an interface for objects that keep generic state related to a viewport
		\author Bart Janssens
*/

#include <k3dsdk/iunknown.h>

namespace k3d
{

// Forward declarations
class iproperty;

/// Abstract interface for objects that can consume time data
class iviewport_state :
	public virtual iunknown
{
public:
	/// The camera used by this viewport
	virtual iproperty& camera_property() = 0;

	/// The OpenGL render engine used by this viewport
	virtual iproperty& render_engine_property() = 0;

	/// Width (in pixels) of the viewport
	virtual iproperty& pixel_width_property() = 0;

	/// Height (in pixels) of the viewport
	virtual iproperty& pixel_height_property() = 0;

	/// Projection matrix related to the current viewport
	virtual iproperty& projection_matrix_property() = 0;

protected:
	iviewport_state() {}
	iviewport_state(const iviewport_state&) {}
	iviewport_state& operator=(const iviewport_state&) { return *this; }
	virtual ~iviewport_state() {}
};

} // namespace k3d

#endif // !K3DSDK_iviewport_state_H

