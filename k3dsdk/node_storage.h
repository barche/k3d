#ifndef K3DSDK_POINTER_DEMAND_STORAGE_H
#define K3DSDK_POINTER_DEMAND_STORAGE_H

// K-3D
// Copyright (c) 1995-2015 K-3D
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

#include <k3dsdk/ihint.h>
#include <k3dsdk/signal_system.h>
#include <k3dsdk/utility.h>

#include <boost/scoped_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <algorithm>
#include <vector>

namespace k3d
{

namespace data
{

/// Storage policy for data containers that store a pointer to a document node that could be deleted by the user at any time - note that value_t should be the interface type used to access the node
template<typename value_t, class signal_policy_t>
class node_storage :
		public signal_policy_t
{
public:
	/// Signal type for a signal that is emitted when the node is about to change from a non-null value
	typedef boost::signals2::signal<void(typename boost::remove_pointer<value_t>::type&)> about_to_change_signal_t;

	/// Returns an interface pointer to the node (could be NULL)
	value_t internal_value()
	{
		return dynamic_cast<value_t>(m_node);
	}

	/// Returns the underlying node (could be NULL)
	inode* internal_node()
	{
		return m_node;
	}

	/// Returns true iff the value is allowed to be NULL
	bool allow_none()
	{
		return true;
	}

	/// Returns true iff the given factory creates an node that could be stored by this container
	bool allow(iplugin_factory& Factory)
	{
		return Factory.implements(typeid(typename boost::remove_pointer<value_t>::type));
	}

	/// Returns true iff the given node coult be stored by this container
	bool allow(inode& Object)
	{
		return dynamic_cast<value_t>(&Object) ? true : false;
	}

	/// Connect here to be notified with the old value (if non-null) when a new node is assigned
	void connect_about_to_change_signal(const typename about_to_change_signal_t::slot_type& Slot)
	{
		m_about_to_change_signal.connect(Slot);
	}

protected:
	template<typename init_t>
	node_storage(const init_t& Init) :
		signal_policy_t(Init),
		m_node(dynamic_cast<inode*>(Init.value()))
	{
		if(m_node)
		{
			m_node_deleted_connection = m_node->deleted_signal().connect(boost::bind(&node_storage::on_node_deleted, this));
			if(inode_change_signal* const node_change_signal = dynamic_cast<inode_change_signal*>(m_node))
				m_node_changed_connection = node_change_signal->connect_node_changed_signal(boost::ref(signal_policy_t::changed_signal()));
		}
	}

	virtual ~node_storage()
	{
	}

	void on_node_deleted()
	{
		internal_set_value(0, 0);
	}

	/// Optionally called to store the original state of the data prior to modification
	void start_recording(istate_recorder& StateRecorder)
	{
		signal_policy_t::start_recording(StateRecorder);
		StateRecorder.current_change_set()->record_old_state(new value_container(m_node));
	}

	/// Sets a new value for the data
	void set_value(value_t Value, ihint* const Hint)
	{
		if(m_node)
		{
			value_t old_value = dynamic_cast<value_t>(m_node);
			if(old_value != nullptr)
			{
				m_about_to_change_signal(*old_value);
			}
			else
			{
				k3d::log() << error << "Invalid value found in node_storage property" << std::endl;
			}
			m_node_deleted_connection.disconnect();
			m_node_changed_connection.disconnect();
		}

		m_node = dynamic_cast<inode*>(Value);

		if(m_node)
		{
			m_node_deleted_connection = m_node->deleted_signal().connect(boost::bind(&node_storage::on_node_deleted, this));
			if(inode_change_signal* const node_change_signal = dynamic_cast<inode_change_signal*>(m_node))
				m_node_changed_connection = node_change_signal->connect_node_changed_signal(boost::ref(signal_policy_t::changed_signal()));
		}

		signal_policy_t::set_value(Hint);
	}

	/// Optionally called to store the new state of the data after one-or-more modifications
	void finish_recording(istate_recorder& StateRecorder)
	{
		StateRecorder.current_change_set()->record_new_state(new value_container(m_node));
		signal_policy_t::finish_recording(StateRecorder);
	}

private:
	/// This little bit of magic allows us to "call" the derived undo policy when our state is modified internally, e.g. if the node is deleted
	virtual void internal_set_value(const value_t&, ihint* const)
	{
	}

	/// Local storage for the node stored by this policy
	inode* m_node;
	boost::signals2::connection m_node_deleted_connection;
	boost::signals2::connection m_node_changed_connection;
	about_to_change_signal_t m_about_to_change_signal;

	/// Provides an implementation of istate_container for storing nodes by value (ValueType must have a copy constructor and assignment operator)
	class value_container :
			public istate_container
	{
	public:
		value_container(inode*& Instance) :
			m_instance(Instance),
			m_value(Instance)
		{
		}

		void restore_state()
		{
			m_instance = m_value;
		}

	private:
		inode*& m_instance;
		inode* m_value;
	};
};


} // namespace data

} // namespace k3d

#endif // !K3DSDK_POINTER_DEMAND_STORAGE_H

