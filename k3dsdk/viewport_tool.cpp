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

#include <k3d-i18n-config.h>

#include <k3dsdk/viewport_tool.h>


namespace k3d
{

viewport_tool::viewport_tool(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
k3d::node(Factory, Document),
m_viewport_state(init_owner(*this) + init_name("viewport_state") + init_label(_("Viewport State")) + init_description(_("Viewport State that provides constext data for this tool")) + init_value(static_cast<k3d::iviewport_state*>(nullptr)))
{
	m_viewport_state.changed_signal().connect(boost::bind(&viewport_tool::on_viewport_state_changed, this, _1));
}

void viewport_tool::on_viewport_state_changed(const k3d::ihint* Hint)
{
	m_events_connection.disconnect();
	k3d::iviewport_state* state = m_viewport_state.pipeline_value();
	if(state != nullptr)
	{
		m_events_connection = state->input_event_signal().connect(boost::bind(&viewport_tool::on_input_event, this, _1));
	}
}

void viewport_tool::on_input_event(const k3d::input_event& Event)
{
	process_input_event(Event);
}

} // namespace k3d
