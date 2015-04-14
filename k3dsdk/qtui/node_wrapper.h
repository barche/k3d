#ifndef K3DSDK_QTUI_NODE_WRAPPER_H
#define K3DSDK_QTUI_NODE_WRAPPER_H

// K-3D
// Copyright (c) 1995-2015, K-3D
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
	\author Bart Janssens (bart@bartjanssens.org)
*/

#include <k3dsdk/iplugin_factory.h>
#include <k3dsdk/inode.h>
#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/type_registry.h>

#include <QObject>
#include <QVariant>

namespace k3d
{

namespace qtui
{

/// Wrapper class for a node, exposing it to QML
class node_wrapper
{
public:
	node_wrapper(iunknown* Node = nullptr);
	node_wrapper(const node_wrapper& Other);

	node_wrapper& operator=(const node_wrapper& Other);

	inode* node() const { return m_node; }
	void set_node(iunknown* Node);

private:
	void on_node_deleted();
	inode* m_node;
	boost::signals2::scoped_connection m_deleted_connection;
};

template<typename T>
T* extract_node(const node_wrapper& Wrapper, const k3d::string_t& PropertyName)
{
	if(Wrapper.node() == nullptr)
		return nullptr;

	T* result = dynamic_cast<T*>(Wrapper.node());
	if(result == nullptr)
		k3d::log() << error << "Node with name [" << Wrapper.node()->name() << "] from factory [" << Wrapper.node()->factory().name() << "] is not of expected type " << k3d::demangle(typeid(T)) << " for property " << PropertyName << std::endl;

	return result;
}

} // namespace qtui

} // namespace k3d
Q_DECLARE_METATYPE(k3d::qtui::node_wrapper)

#endif // !K3DSDK_QTUI_NODE_WRAPPER_H

