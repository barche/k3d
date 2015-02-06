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

/** \file
	\author Bart Janssens
*/

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/property.h>
#include <k3dsdk/transform.h>

#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/node_wrapper.h>

namespace k3d
{

namespace qtui
{

node_wrapper::node_wrapper(iunknown* Node)
{
	set_node(Node);
}

node_wrapper::node_wrapper(const node_wrapper &Other)
{
	set_node(Other.m_node);
}

node_wrapper& node_wrapper::operator=(const node_wrapper& Other)
{
	set_node(Other.m_node);
}

void node_wrapper::set_node(iunknown* Node)
{
	m_deleted_connection.disconnect();
	m_node = nullptr;
	if(Node == nullptr)
	{
		return;
	}

	m_node = dynamic_cast<inode*>(Node);
	if(m_node == nullptr)
	{
		k3d::log() << error << "Object passed to node_wrapper is not a node" << std::endl;
		return;
	}

	m_deleted_connection = m_node->deleted_signal().connect(boost::bind(&node_wrapper::on_node_deleted, this));
}

void node_wrapper::on_node_deleted()
{
	m_node = nullptr;
}


} // namespace qtui

} // namespace k3d

