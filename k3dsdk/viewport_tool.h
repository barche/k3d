#ifndef K3DSDK_VIEWPORT_TOOL_H
#define K3DSDK_VIEWPORT_TOOL_H

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

#include <k3dsdk/data.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node_storage.h>
#include <k3dsdk/input_event.h>
#include <k3dsdk/iviewport_state.h>
#include <k3dsdk/iviewport_tool.h>


namespace k3d
{

/// Boilerplate implementation for iviewport_tool
class viewport_tool : public k3d::node, public k3d::iviewport_tool
{
public:
  viewport_tool(k3d::iplugin_factory& Factory, k3d::idocument& Document);

	/// Returns the property that contains the viewport state
	virtual iproperty& viewport_state_property()
	{
		return m_viewport_state;
	}

protected:
	// Implement this to receive an input event
	virtual void process_input_event(const k3d::input_event& Event) = 0;
	k3d_data(k3d::iviewport_state*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_viewport_state;
private:
  void on_viewport_state_changed(const k3d::ihint* Hint);
	void on_input_event(const k3d::input_event& Event);

	boost::signals2::connection m_events_connection;
};

} // namespace k3d

#endif // !K3DSDK_VIEWPORT_TOOL_H
