#ifndef K3DSDK_SYSTEM_H
#define K3DSDK_SYSTEM_H

// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
	\author Romain Behar (romainbehar@yahoo.com)
	\author Timothy M. Shead (tshead@k-3d.com)
*/

#include <boost/filesystem/path.hpp>
#include <k3dsdk/types.h>

#include <map>
#include <vector>

namespace k3d
{

namespace system
{

/// Used to initialize the current executable path.  Should be called once as soon as possible at startup, before the cwd can be altered.
void initialize_executable_path(int argc, char* argv[]);
/// Returns the absolute path of the current executable, or an empty path
const boost::filesystem::path executable_path();

/// Safely returns an environment variable (returns empty string if the variable doesn't exist)
const string_t getenv(const string_t& Variable);
/// Safely sets an environment variable using separate name and value strings
void setenv(const string_t& Name, const string_t& Value);
/// Safely sets an environment variable using "NAME=VALUE" syntax
void setenv(const string_t& Variable);

/// Safely returns the user's home directory
const boost::filesystem::path get_home_directory();
/// Safely returns the user's options directory
const boost::filesystem::path get_options_directory();
/// Safely returns the user's temp directory
const boost::filesystem::path get_temp_directory();
/// Returns a unique temporary file path
const boost::filesystem::path generate_temp_file();

/// Returns the path to a binary executable by searching the contents of the PATH environment variable, or an empty path
const boost::filesystem::path find_executable(const string_t& Executable);

/// Returns the most recent modification time of a file
bool file_modification_time(const boost::filesystem::path& File, time_t& ModificationTime);

/// Indicate how a spawned process should be treated
enum spawn_type
{
	SPAWN_SYNCHRONOUS = 0, // Main program waits
	SPAWN_ASYNCHRONOUS = 1 // Separate thread waits on command completion
};

/// Type for the environment passed to spawn
typedef std::map<string_t, string_t> environment_t;

/// Runs an external process in the same environment as the main program. Note: execs the process directly, do not use shell features!
/// @param Command is the file to execute, if it does not contain a '/' it is looked up in $PATH
/// @param Arguments contains the program arguments (Command must NOT be included as the first element here)
/// Program output is placed in StandardOut and StandardError
/// @param WorkingDirectory is the directory that is changed to before executing Command
bool spawn(const boost::filesystem::path& Command, const std::vector<string_t>& Arguments, const spawn_type SpawnType, string_t& StandardOut, string_t& StandardError, const boost::filesystem::path& WorkingDirectory = boost::filesystem::path(), const environment_t& Environment = environment_t());

/// Defines a collection of paths
typedef std::vector<boost::filesystem::path> paths_t;
/// Split a string containing zero-or-more paths separated by delimiters into a collection of paths ...
const paths_t decompose_path_list(const string_t Input);

/// Returns the correct name for an executable on the local platform
const string_t executable_name(const string_t& Executable);

/// Blocks the calling thread for the given number of seconds
void sleep(const double Seconds);

} // namespace system

} // namespace k3d

#endif // !K3DSDK_SYSTEM_H

