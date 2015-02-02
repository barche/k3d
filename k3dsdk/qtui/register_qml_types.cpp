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

/** \file Register QML types
	\author Bart Janssens (bart@bartjanssens.org)
*/

#include <k3dsdk/qtui/property_wrapper.h>

#include <QtQml>

namespace k3d
{

namespace qtui
{

struct qml_type_registry
{
	qml_type_registry()
	{
		// Register QML types
		qRegisterMetaType<property_wrapper*>("property_wrapper*");
	}
};

qml_type_registry registry;


} // namespace qtui

} // namespace k3d

