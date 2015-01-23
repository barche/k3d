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
	\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3d-platform-config.h>
#include <k3d-version-config.h>

#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/system.h>
#include <k3dsdk/utility.h>

#include <boost/filesystem.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

namespace detail
{

typedef std::vector<std::string> string_array;

bool g_show_timestamps = false;
bool g_show_process = true;
bool g_syslog = false;
bool g_color_level = false;
k3d::log_level_t g_minimum_log_level = k3d::K3D_LOG_LEVEL_DEBUG;

/////////////////////////////////////////////////////////////////////////////
// render_job

bool render_job(const boost::filesystem::path& JobDirectory)
{
	// Sanity checks ...
	if(!boost::filesystem::exists(JobDirectory))
	{
		k3d::log() << error << "Job directory " << JobDirectory.native() << " does not exist" << std::endl;
		return false;
	}

	if(!boost::filesystem::is_directory(JobDirectory))
	{
		k3d::log() << error << "Job directory " << JobDirectory.native() << " is not a directory" << std::endl;
		return false;
	}

	// Skip the job if it's complete ...
	if(boost::filesystem::exists(JobDirectory / boost::filesystem::path("complete")))
		return true;

	// Skip the job if it errored out ...
	if(boost::filesystem::exists(JobDirectory / boost::filesystem::path("error")))
		return true;

	// Skip the job if it's running ...
	if(boost::filesystem::exists(JobDirectory / boost::filesystem::path("running")))
		return true;

	// Make sure the job is ready ...
	if(!boost::filesystem::exists(JobDirectory / boost::filesystem::path("ready")))
	{
		k3d::log() << error << "Job " << JobDirectory.native() << " is not ready" << std::endl;
		return false;
	}

	// Standard logging ...
	k3d::log() << info << "Starting Job " << JobDirectory.native() << std::endl;

	// Switch the job status to running ...
	boost::filesystem::rename(JobDirectory / boost::filesystem::path("ready"), JobDirectory / boost::filesystem::path("running"));

	// For each directory in the job directory (non-recursive) ...
	for(boost::filesystem::directory_iterator frame(JobDirectory); frame != boost::filesystem::directory_iterator(); ++frame)
	{
		if(!boost::filesystem::is_directory(*frame))
			continue;

		k3d::string_t std_out, std_err;
		if(!k3d::system::spawn(boost::filesystem::path("k3d-renderframe"), std::vector<k3d::string_t>(1, frame->path().native()), k3d::system::SPAWN_SYNCHRONOUS, std_out, std_err))
		{
			k3d::log() << error << "Renderjob failed with message: " << std_err << std::endl;
		}
		k3d::log() << debug << "k3d-renderframe output was:\n" << std_out << std::endl;
	}

	// Switch the job status to complete ...
	boost::filesystem::rename(JobDirectory / boost::filesystem::path("running"), JobDirectory / boost::filesystem::path("complete"));

	// Standard logging ...
	k3d::log() << info << "Completed Job " << JobDirectory.native() << std::endl;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// usage

/// Prints usage info
void usage(const std::string& Name, std::ostream& Stream)
{
	Stream << "usage: " << Name << " [options]" << std::endl;
	Stream << "       " << Name << " [directory ...]" << std::endl;
	Stream << std::endl;
	Stream << "  -h, --help               prints this help information and exits" << std::endl;
	Stream << "      --version            prints program version information and exits" << std::endl;
	Stream << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// version

/// Prints version info
void print_version(std::ostream& Stream)
{
	Stream << "K-3D Version " << K3D_VERSION << std::endl;
	Stream << K3D_COPYRIGHT << "  See the AUTHORS file for contributors." << std::endl;
	Stream << "Licensed by the GNU General Public License.  See the COPYING file for details." << std::endl;
	Stream << "K-3D Home Page: http://www.k-3d.org" << std::endl;
	Stream << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
// setup_logging

/// Sets-up options for logging our output
void setup_logging(const std::string& ProcessName)
{
	k3d::log_show_timestamps(g_show_timestamps);
	k3d::log_set_tag(g_show_process ? "[" + ProcessName + "]" : std::string());
	k3d::log_color_level(g_color_level);
	k3d::log_show_level(true);
	k3d::log_syslog(g_syslog);
	k3d::log_minimum_level(g_minimum_log_level);
}

} // namespace detail

/// Program main
int main(int argc, char* argv[])
{
#ifndef K3D_API_WIN32
	// Fork ourselves so we don't become a zombie of the K-3D executable
	const int fork_result = fork();
	if(fork_result < 0)
		return 1;
	else if(fork_result > 0)
		return 0;
#endif // !K3D_API_WIN32

	const std::string program_name = boost::filesystem::path(k3d::string_t(std::string(argv[0]))).leaf().native();

	// Put our command-line arguments in a more useable form ...
	detail::string_array options(&argv[1], &argv[argc]);

	// Print a "help" message ...
	if(std::count(options.begin(), options.end(), "-h") || std::count(options.begin(), options.end(), "--help"))
	{
		detail::usage(program_name, std::cout);
		return 0;
	}

	// Print version data ...
	if(std::count(options.begin(), options.end(), "--version"))
	{
		detail::print_version(std::cout);
		return 0;
	}

	// Otherwise we should have a minimum of one argument ...
	if(options.size() < 1)
	{
		detail::usage(program_name, k3d::log());
		return 1;
	}

	// Setup logging right away ...
	detail::setup_logging(program_name);

	// Each remaining argument should be a job path to render ...
	int result = 0;
	for(unsigned long j = 0; j < options.size(); j++)
	{
		if(!detail::render_job(boost::filesystem::path(k3d::string_t(options[j]))))
			result = 1;
	}

	return result;
}

