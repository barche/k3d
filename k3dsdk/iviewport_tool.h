#ifndef K3DSDK_IVIEWPORT_TOOL_H
#define K3DSDK_IVIEWPORT_TOOL_H

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
		\brief Declares iviewport_tool, an interface for tools that interactively operate in a viewport
		\author Tim Shead (tshead@k-3d.com)
		\author Bart Janssens
*/

#include <k3dsdk/iunknown.h>


namespace k3d
{

// Forward declarations
class iproperty;

/// Abstract interface for objects that can consume time data
class iviewport_tool :
	public virtual iunknown
{
public:
	/// Returns the property that contains the viewport state
	virtual iproperty& viewport_state_property() = 0;

protected:
	iviewport_tool() {}
	iviewport_tool(const iviewport_tool&) {}
	iviewport_tool& operator=(const iviewport_tool&) { return *this; }
	virtual ~iviewport_tool() {}
};

} // namespace k3d

#endif // !K3DSDK_IVIEWPORT_TOOL_H

