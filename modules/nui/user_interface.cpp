// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/iuser_interface.h>
#include <k3dsdk/ievent_loop.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>

#include <iostream>

namespace module
{

namespace nui
{

/////////////////////////////////////////////////////////////////////////////
// user_interface

/// Provides the Null User Interface, a do-nothing user interface plugin useful for executing scripts and as a starting-point for user interface authors
class user_interface :
	public k3d::ievent_loop,
	public k3d::iuser_interface
{
public:
	void get_command_line_arguments(boost::program_options::options_description& Description)
	{
	}

	const arguments_t parse_startup_arguments(const arguments_t& Arguments, k3d::bool_t& Quit, k3d::bool_t& Error)
	{
		return Arguments;
	}

	const arguments_t parse_runtime_arguments(const arguments_t& Arguments, k3d::bool_t& Quit, k3d::bool_t& Error)
	{
		return Arguments;
	}

	void startup_message_handler(const k3d::string_t& Message)
	{
	}

	void display_user_interface()
	{
	}

	void start_event_loop()
	{
	}

	void stop_event_loop()
	{
	}

	void open_uri(const k3d::string_t& URI)
	{
	}

	void message(const k3d::string_t& Message)
	{
		std::cout << "MESSAGE: " << Message << std::endl;
	}

	void warning_message(const k3d::string_t& Message)
	{
		std::cout << "WARNING: " << Message << std::endl;
	}

	void error_message(const k3d::string_t& Message)
	{
		std::cout << "ERROR: " << Message << std::endl;
	}

	k3d::uint_t query_message(const k3d::string_t& Message, const k3d::uint_t DefaultOption, const std::vector<k3d::string_t>& Options)
	{
		return 0;
	}

	void nag_message(const k3d::string_t& Type, const k3d::string_t& Message, const k3d::string_t& SecondaryMessage)
	{
		std::cout << "MESSAGE: " << Message << " " << SecondaryMessage << std::endl;
	}

	k3d::bool_t get_file_path(const k3d::ipath_property::mode_t Mode, const k3d::string_t& Type, const k3d::string_t& Prompt, const boost::filesystem::path& OldPath, boost::filesystem::path& Result)
	{
		return false;
	}

	k3d::bool_t show(iunknown& Object)
	{
		return false;
	}

	void synchronize()
	{
	}

	boost::signals2::connection get_timer(const k3d::double_t FrameRate, const k3d::void_signal_t::slot_type& Slot)
	{
		return boost::signals2::connection();
	}
	
	k3d::uint_t watch_path(const boost::filesystem::path& Path, const k3d::void_signal_t::slot_type& Slot)
	{	
		return 0;
	}

	void unwatch_path(const k3d::uint_t WatchID)
	{
	}

	k3d::iplugin_factory& factory()
	{
		return get_factory();
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::application_plugin_factory<user_interface,
			k3d::interface_list<k3d::ievent_loop> > factory(
			k3d::uuid(0xe6ade3f1, 0x484ce7b1, 0x1c118cb2, 0x9a3da138),
			"NUI",
			"Null User Interface (NUI)",
			"Interface");

		return factory;
	}
};

} // namespace nui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::nui::user_interface::get_factory());
K3D_MODULE_END


