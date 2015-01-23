// K-3D
// Copyright (c) 1995-2008, Timothy M. Shead
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
	\author Tim Shead (tshead@k-3d.com)
*/

#include <k3d-platform-config.h>
#include <k3d-version-config.h>

#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/system.h>
#include <k3dsdk/types.h>
#include <k3dsdk/utility.h>

#include <k3dsdk/xml.h>
using namespace k3d::xml;

#ifdef K3D_API_WIN32

	#include <k3dsdk/win32.h>
	#include <direct.h>
	#define chdir _chdir

#endif // K3D_API_WIN32

#include <glibmm/spawn.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include <cassert>
#include <ctime>
#include <iostream>
#include <vector>

extern char** environ;

namespace detail
{

typedef std::vector<k3d::string_t> string_array;

bool g_show_timestamps = false;
bool g_show_process = true;
bool g_syslog = false;
bool g_color_level = true;
k3d::log_level_t g_minimum_log_level = k3d::K3D_LOG_LEVEL_DEBUG;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// expand

const k3d::string_t expand(const k3d::string_t& Value)
{
	static boost::regex variable_regex("[$]([^$].*)[$]");
	static std::map<k3d::string_t, k3d::string_t> builtin_variables;

	k3d::string_t value = Value;
	for(boost::sregex_iterator variable(value.begin(), value.end(), variable_regex); variable != boost::sregex_iterator(); ++variable)
	{
		const k3d::string_t variable_expression = (*variable)[0].str();
		const k3d::string_t variable_name = (*variable)[1].str();

		k3d::string_t variable_value;
		if(builtin_variables.count(variable_name))
		{
			variable_value = builtin_variables[variable_name];
		}
		else
		{
			variable_value = k3d::system::getenv(variable_name);
		}

		boost::algorithm::replace_first(value, variable_expression, variable_value);
	}
	return value;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// exec_command

bool exec_command(const element& XMLCommand, const boost::filesystem::path& FrameDirectory)
{
	k3d::string_t working_directory = FrameDirectory.native();
	std::vector<k3d::string_t> arguments;
	k3d::system::environment_t environment;
	k3d::string_t standard_output;
	k3d::string_t standard_error;
	int exit_status = 0;

	const std::string binary = attribute_text(XMLCommand, "binary");

	// Setup program arguments ...
	if(const element* const xml_arguments = find_element(XMLCommand, "arguments"))
	{
		for(element::elements_t::const_iterator xml_argument = xml_arguments->children.begin(); xml_argument != xml_arguments->children.end(); ++xml_argument)
		{
			k3d::string_t value = expand(attribute_text(*xml_argument, "value"));
			arguments.push_back(value);
		}
	}

	// Setup the application environment ...
	for(int i = 0; environ[i]; ++i)
	{
		k3d::string_t env_var_str(environ[i]);
		const std::size_t equals_pos = env_var_str.find_first_of("=");
		environment[env_var_str.substr(0, equals_pos)] = env_var_str.substr(equals_pos+1, std::string::npos);
	}

	if(const element* const xml_environment = find_element(XMLCommand, "environment"))
	{
		for(element::elements_t::const_iterator xml_variable = xml_environment->children.begin(); xml_variable != xml_environment->children.end(); ++xml_variable)
		{
			k3d::string_t name = attribute_text(*xml_variable, "name");
			k3d::string_t value = expand(attribute_text(*xml_variable, "value"));

			environment[name] = value;
		}
	}

#ifdef K3D_API_WIN32
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
  ZeroMemory( &pi, sizeof(pi) );
  
  k3d::string_t command_line;
  for(std::vector<k3d::string_t>::const_iterator argument = arguments.begin(); argument != arguments.end(); ++argument)
  	command_line += *argument + " ";
  
  k3d::log() << info << "Executing " << command_line << std::endl;
  
  std::vector<char> env;
  for(std::vector<k3d::string_t>::const_iterator el = environment.begin(); el != environment.end(); ++el)
  {
  	for(k3d::uint_t i = 0; i != el->size(); ++i)
  		env.push_back(el->at(i));
  	env.push_back('\0');
  }
  env.push_back('\0');
  
  // Start the child process. 
  if( !CreateProcess( NULL,   // No module name (use command line)
  		const_cast<char*>(command_line.c_str()),        // Command line
      NULL,           // Process handle not inheritable
      NULL,           // Thread handle not inheritable
      FALSE,          // Set handle inheritance to FALSE
      CREATE_NO_WINDOW,              // Don't create a DOS window
      &env[0],
      const_cast<char*>(working_directory.c_str()),            
      &si,            // Pointer to STARTUPINFO structure
      &pi )           // Pointer to PROCESS_INFORMATION structure
  ) 
  {
      k3d::log() << error << "CreateProcess failed " << GetLastError() << std::endl;
      return false;
  }

  // Wait until child process exits.
  WaitForSingleObject( pi.hProcess, INFINITE );

  // Close process and thread handles. 
  CloseHandle( pi.hProcess );
  CloseHandle( pi.hThread );
  
  return true;
#else
	
	try
	{
		k3d::log() << info;
		for(const auto& env_entry : environment) { k3d::log() << " " << env_entry.first << "=" << env_entry.second; }
		std::copy(arguments.begin(), arguments.end(), std::ostream_iterator<k3d::string_t>(k3d::log(), " "));
		k3d::log() << std::endl;

		k3d::system::spawn(boost::filesystem::path(binary), arguments, k3d::system::SPAWN_SYNCHRONOUS, standard_output, standard_error, boost::filesystem::path(working_directory), environment);

		if(!standard_output.empty())
			k3d::log() << info << "stdout: " << standard_output << std::endl;

		if(!standard_error.empty())
			k3d::log() << error << "stderr: " << standard_error << std::endl;

		return true;
	}
	catch(Glib::SpawnError& e)
	{
		k3d::log() << error << e.what() << std::endl;
	}
	catch(...)
	{
		k3d::log() << error << "caught unknown exception spawning process" << std::endl;
	}

	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// copy_command

bool copy_command(const element& XMLCommand, const boost::filesystem::path& FrameDirectory)
{
	try
	{
		const boost::filesystem::path source = boost::filesystem::path(k3d::string_t(attribute_text(XMLCommand, "source")));
		const boost::filesystem::path target = boost::filesystem::path(k3d::string_t(attribute_text(XMLCommand, "target")));

		boost::filesystem::remove(target);
		boost::filesystem::copy_file(source, target);

		return true;
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "exception copying file: " << e.what();
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// view_command

bool view_command(const element& XMLCommand, const boost::filesystem::path& FrameDirectory)
{
	const k3d::string_t path = attribute_text(XMLCommand, "file");

#ifndef K3D_API_WIN32

/*
	// View the image ...
	boost::format command_line(k3d::options::get_command(k3d::options::command::bitmap_viewer()));
	command_line % path;

	// Execute the command ...
	k3d::system::spawn_async(command_line.str());
*/

#else // !K3D_API_WIN32

	ShellExecute(0, "open", path.c_str(), 0, 0, SW_SHOWDEFAULT);

#endif // K3D_API_WIN32

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// execute_command

/// Handles a single command during processing for the frame ...
bool execute_command(const element& XMLCommand, const boost::filesystem::path& FrameDirectory)
{
	for(element::elements_t::const_iterator xml_command = XMLCommand.children.begin(); xml_command != XMLCommand.children.end(); ++xml_command)
	{
		if(xml_command->name == "exec")
			return exec_command(*xml_command, FrameDirectory);
		else if(xml_command->name == "copy")
			return copy_command(*xml_command, FrameDirectory);
		else if(xml_command->name == "view")
			return view_command(*xml_command, FrameDirectory);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// render_frame

/// Handles all processing for the given frame
bool render_frame(const boost::filesystem::path& FrameDirectory)
{
	if(!boost::filesystem::exists(FrameDirectory))
	{
		k3d::log() << error << "Frame directory " << FrameDirectory.native() << " does not exist" << std::endl;
		return false;
	}

	if(!boost::filesystem::is_directory(FrameDirectory))
	{
		k3d::log() << error << "Frame directory " << FrameDirectory.native() << " is not a directory" << std::endl;
		return false;
	}

	// Skip the frame if it's complete ...
	if(boost::filesystem::exists(FrameDirectory / boost::filesystem::path("complete")))
		return true;

	// Skip the frame if it errored out ...
	if(boost::filesystem::exists(FrameDirectory / boost::filesystem::path("error")))
		return true;

	// Skip the frame if it's running ...
	if(boost::filesystem::exists(FrameDirectory / boost::filesystem::path("running")))
		return true;

	// Make sure the frame is ready ...
	if(!boost::filesystem::exists(FrameDirectory / boost::filesystem::path("ready")))
	{
		k3d::log() << error << "Frame " << FrameDirectory.native() << " is not ready" << std::endl;
		return false;
	}

	// Standard logging ...
	k3d::log() << info << "Starting Frame " << FrameDirectory.native() << std::endl;

	// Switch the frame status to running ...
	boost::filesystem::rename(FrameDirectory / boost::filesystem::path("ready"), FrameDirectory / boost::filesystem::path("running"));

	// Load the frame options file ...
	element xml_frame_options("empty");
	const boost::filesystem::path control_file_path = FrameDirectory / boost::filesystem::path("control.k3d");
	try
	{
		boost::filesystem::ifstream stream(control_file_path);
		hide_progress progress;
		parse(xml_frame_options, stream, control_file_path.native(), progress);
	}
	catch(std::exception& e)
	{
		k3d::log() << error << "Frame " << FrameDirectory.native() << " error parsing control file " << control_file_path.native() << " " << e.what() << std::endl;
		return false;
	}

	// Get the frame data ...
	element* const xml_frame = find_element(xml_frame_options, "frame");
	if(!xml_frame)
	{
		k3d::log() << error << "Missing <frame> data in control file " << control_file_path.native() << std::endl;
		return false;
	}

	// Setup our execution environment ...
	chdir(FrameDirectory.native().c_str());

	for(element::elements_t::iterator xml_command = xml_frame->children.begin(); xml_command != xml_frame->children.end(); ++xml_command)
		execute_command(*xml_command, FrameDirectory);

	// Switch the frame status to complete ...
	boost::filesystem::rename(FrameDirectory / boost::filesystem::path("running"), FrameDirectory / boost::filesystem::path("complete"));

	// Standard logging ...
	k3d::log() << info << "Completed Frame " << FrameDirectory.native() << std::endl;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// usage

/// Prints usage info
void usage(const k3d::string_t& Name, std::ostream& Stream)
{
	Stream << "usage: " << Name << " [options]" << std::endl;
	Stream << "       " << Name << " [directory ...]" << std::endl;
	Stream << std::endl;
	Stream << "  -h, --help               prints this help information and exits" << std::endl;
	Stream << "      --version            prints program version information and exits" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// version

/// Prints version info
void print_version(std::ostream& Stream)
{
	Stream << "K-3D version " << K3D_VERSION << std::endl;
	Stream << K3D_COPYRIGHT << "  See the AUTHORS file for contributors." << std::endl;
	Stream << "Licensed by the GNU General Public License.  See the COPYING file for details." << std::endl;
	Stream << "K-3D Home Page: http://www.k-3d.org" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// setup_logging

/// Sets-up options for logging our output
void setup_logging(const k3d::string_t& ProcessName)
{
	k3d::log_show_timestamps(g_show_timestamps);
	k3d::log_set_tag(g_show_process ? "[" + ProcessName + "]" : k3d::string_t());
	k3d::log_color_level(g_color_level);
	k3d::log_show_level(true);
	k3d::log_syslog(g_syslog);
	k3d::log_minimum_level(g_minimum_log_level);
}

} // namespace detail

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main

/// Program main
int main(int argc, char* argv[])
{
	const k3d::string_t program_name = boost::filesystem::path(k3d::string_t(k3d::string_t(argv[0]))).leaf().native();

	// Put our arguments in a more useable form ...
	detail::string_array options(&argv[1], &argv[argc]);

	// Print a "help" message ...
	if(std::count(options.begin(), options.end(), "-h") || std::count(options.begin(), options.end(), "--help"))
	{
		detail::usage(program_name, std::cout);
		return 0;
	}

	// Print version data ...
	if(options.end() != std::find(options.begin(), options.end(), "--version"))
	{
		detail::print_version(std::cout);
		return 0;
	}

	// Otherwise we should have a minimum of two arguments ...
	if(options.size() < 1)
	{
		detail::usage(program_name, k3d::log());
		return 1;
	}

	// Setup logging right away ...
	detail::setup_logging(program_name);

	// Each remaining argument should be a frame path to render ...
	int result = 0;
	for(unsigned long j = 0; j < options.size(); j++)
	{
		if(!detail::render_frame(boost::filesystem::path(k3d::string_t(options[j]))))
		    result = 1;
	}

	return result;
}

