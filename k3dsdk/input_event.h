#ifndef K3DSDK_INPUT_EVENT_H
#define K3DSDK_INPUT_EVENT_H

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
		\brief Declares input_event, to represent user input events
		\author Bart Janssens
*/

#include <k3dsdk/algebra.h>


namespace k3d
{

enum class input_modifier : int
{
	NONE = 1L << 0,
	SHIFT = 1L << 1,
	META_OR_CTRL = 1L << 2, // ctrl on OS X, meta (Windows key) on others
	CTRL_OR_COMMAND = 1L << 3, // command on OS X, ctrl on others
	ALT = 1L << 4
};

/// Allow use as bitmask on modifier
inline input_modifier
operator&(input_modifier A, input_modifier B)
{ return static_cast<input_modifier>(static_cast<int>(A) & static_cast<int>(B)); }

inline input_modifier
operator|(input_modifier A, input_modifier B)
{ return static_cast<input_modifier>(static_cast<int>(A) | static_cast<int>(B)); }

inline input_modifier
operator^(input_modifier A, input_modifier B)
{ return static_cast<input_modifier>(static_cast<int>(A) ^ static_cast<int>(B)); }

inline input_modifier
operator~(input_modifier A)
{ return static_cast<input_modifier>(~static_cast<int>(A)); }

inline const input_modifier&
operator|=(input_modifier& A, input_modifier B)
{ return A = A | B; }

inline const input_modifier&
operator&=(input_modifier& A, input_modifier B)
{ return A = A & B; }

inline const input_modifier&
operator^=(input_modifier& A, input_modifier B)
{ return A = A ^ B; }

enum class input_type
{
	UNKNOWN,
	BUTTON_PRESSED,
	BUTTON_RELEASED,
	POSITION_CHANGE
};

enum class input_button
{
	UNKNOWN,
	NONE,
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE
};

/// Encapsulate a user event
struct input_event
{
	input_type type = input_type::UNKNOWN;
	input_button button = input_button::NONE;
	input_modifier modifier = input_modifier::NONE;

	// Mouse position for the event
	k3d::point2 position;
};

} // namespace k3d

#endif // !K3DSDK_INPUT_EVENT_H
