// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

/** \file Functions for handling translation between Qt and K3D events
	\author Bart Janssens
*/

#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>

#include <k3dsdk/qtui/events.h>

#include <QMouseEvent>

#include <boost/assign/list_of.hpp>
#include <map>

namespace k3d
{

namespace qtui
{

namespace detail
{

template<typename MapT>
typename MapT::mapped_type map_lookup(const MapT& Map, const typename MapT::key_type& Key)
{
	const typename MapT::const_iterator it = Map.find(Key);
	if(it == Map.end())
	{
		k3d::log() << error << "Key " << Key << " not found in Qt->K3D event conversion map" << std::endl;
		return MapT::mapped_type::UNKNOWN;
	}

	return it->second;
}

k3d::input_type convert_event_type(const QEvent::Type Type)
{
	static std::map<QEvent::Type, k3d::input_type> type_map = boost::assign::map_list_of
	(QEvent::MouseButtonPress, k3d::input_type::BUTTON_PRESSED)
	(QEvent::MouseButtonRelease, k3d::input_type::BUTTON_RELEASED)
	(QEvent::MouseMove, k3d::input_type::POSITION_CHANGE);

	return map_lookup(type_map, Type);
}

k3d::input_button convert_button(const Qt::MouseButton Button)
{
	static std::map<Qt::MouseButton, k3d::input_button> button_map = boost::assign::map_list_of
	(Qt::NoButton, k3d::input_button::NONE)
	(Qt::LeftButton, k3d::input_button::MOUSE_LEFT)
	(Qt::RightButton, k3d::input_button::MOUSE_RIGHT);

	return map_lookup(button_map, Button);
}

k3d::input_modifier convert_modifier(const Qt::KeyboardModifiers Modifier)
{
	k3d::input_modifier result = k3d::input_modifier::NONE;
	if(Modifier == Qt::NoModifier)
		return result;

	if(Modifier & Qt::ShiftModifier)
		result |= k3d::input_modifier::SHIFT;

	if(Modifier & Qt::ControlModifier)
		result |= k3d::input_modifier::CTRL_OR_COMMAND;

	if(Modifier & Qt::AltModifier)
		result |= k3d::input_modifier::ALT;

	if(Modifier & Qt::MetaModifier)
		result |= k3d::input_modifier::META_OR_CTRL;

	return result;
}

}

k3d::input_event make_k3d_event(const QInputEvent& Event)
{
	k3d::input_event k3d_event;

	k3d_event.type = detail::convert_event_type(Event.type());
	k3d_event.modifier = detail::convert_modifier(Event.modifiers());

	const QMouseEvent* mouse_event = dynamic_cast<const QMouseEvent*>(&Event);
	if(mouse_event != nullptr)
		k3d_event.button = detail::convert_button(mouse_event->button());

	return k3d_event;
}

} // namespace qtui

} // namespace k3d
