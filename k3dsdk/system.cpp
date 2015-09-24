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
		\brief Implements system related functions
		\author Tim Shead (tshead@k-3d.com)
*/

#include <errno.h>
#include <boost/filesystem/path.hpp>
#include <k3d-platform-config.h>
#include <k3dsdk/result.h>
#include <k3dsdk/string_modifiers.h>
#include <k3dsdk/system.h>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <cmath>
#include <cstring>

#include <sys/stat.h>

// Define some platform-specific odds-and-ends
#if defined K3D_API_WIN32

	#define SEARCHPATH_SEPARATOR_STRING ";"
	#define DEFAULT_TEMP_DIRECTORY "c:\\"
	#define DEFAULT_HOME_DIRECTORY "c:\\"
	#define popen _popen

	#include <k3dsdk/win32.h>
	#include <io.h>

#else // K3D_API_WIN32

	#define SEARCHPATH_SEPARATOR_STRING ":"
	#define DEFAULT_TEMP_DIRECTORY "/tmp"
	#define DEFAULT_HOME_DIRECTORY "/"

	#include <time.h>
	#include <unistd.h>
	#include <sys/wait.h>

#endif // !K3D_API_WIN32

#if defined K3D_API_DARWIN

	#include <mach-o/dyld.h>

#endif // K3D_API_DARWIN

namespace k3d
{

namespace system
{

namespace detail
{

/// Construct the argv char array from a command and a vector of strings
struct argv_from_vector
{
	argv_from_vector(const std::string& Command, std::vector<std::string> Arguments) : nb_args(Arguments.size() + 1)
	{
		// Integrate the Command into the Arguments
		Arguments.insert(Arguments.begin(), Command);
		argv = new char*[nb_args+1];
		for(k3d::uint_t i = 0; i != nb_args; ++i)
		{
			const std::string& argument = Arguments[i];
			const k3d::uint_t str_size = argument.size();
			argv[i] = new char[str_size+1];
			argument.copy(argv[i], str_size);
			argv[i][str_size] = '\0';
		}
		argv[nb_args] = static_cast<char*>(0);
	}

	~argv_from_vector()
	{
		for(k3d::uint_t i = 0; i != nb_args; ++i)
			delete[] argv[i];
		delete[] argv;
	}

	char** argv;
	const k3d::uint_t nb_args;
};

} // namespace detail

static boost::filesystem::path g_executable_path;

void initialize_executable_path(int argc, char* argv[])
{
#if defined K3D_API_WIN32

	string_t buffer(256, '\0');
	GetModuleFileName(0, const_cast<char*>(buffer.data()), buffer.size());
	buffer.resize(strlen(buffer.c_str()));
	g_executable_path = boost::filesystem::path(string_t(buffer));
	return;

#elif defined K3D_API_DARWIN

	string_t buffer;
	uint32_t buffer_size = 0;
	_NSGetExecutablePath(const_cast<char*>(buffer.data()), &buffer_size);
	buffer.resize(buffer_size);
	_NSGetExecutablePath(const_cast<char*>(buffer.data()), &buffer_size);
	g_executable_path = boost::filesystem::path(string_t(buffer));
	return;

#else
	// Linux ...
	if(boost::filesystem::exists(boost::filesystem::path(string_t("/proc/self/exe"))))
	{
		string_t buffer(256, '\0');
		readlink("/proc/self/exe", const_cast<char*>(buffer.data()), buffer.size());
		g_executable_path = boost::filesystem::path(string_t(buffer));
		return;
	}

	// BSD ...
	if(boost::filesystem::exists(boost::filesystem::path(string_t("/proc/curproc/file"))))
	{
		string_t buffer(256, '\0');
		readlink("/proc/curproc/file", const_cast<char*>(buffer.data()), buffer.size());
		g_executable_path = boost::filesystem::path(string_t(buffer));
		return;
	}
#endif
}

const boost::filesystem::path executable_path()
{
	if(g_executable_path.empty())
		log() << warning << "Uninitialized executable path ... did you forget to call initialize_executable_path(...) at startup?" << std::endl;

	return g_executable_path;
}

const string_t getenv(const string_t& Variable)
{
	if(const char* variable = ::getenv(Variable.c_str()))
		return string_t(variable);

	return string_t();
}

void setenv(const string_t& Name, const string_t& Value)
{
	int retval = 0;

#ifdef K3D_API_WIN32

	// Use putenv on Win32 because it's the only thing available, and it copies its inputs
	retval = ::putenv((Name + "=" + Value).c_str());

#else // K3D_API_WIN32

	// Use setenv where possible because it copies its inputs
	retval = ::setenv(Name.c_str(), Value.c_str(), true);

#endif // !K3D_API_WIN32

	if(retval == -1)
	{
		k3d::log() << error << "Error  " << errno << " when setting environment variable " << Name << " to value " << Value << ": " << strerror(errno) << std::endl;
	}
}

void setenv(const string_t& Variable)
{
	int retval = 0;

#ifdef K3D_API_WIN32

	// Use putenv on Win32 because it's the only thing available, and it copies its inputs
	retval = ::putenv(Variable.c_str());

#else // K3D_API_WIN32

	// On unix, we have to make a copy of the string to pass to putenv()
	retval = ::putenv(::strdup(Variable.c_str()));

#endif // !K3D_API_WIN32

	if(retval == -1)
	{
		k3d::log() << error << "Error  " << errno << " when setting environment variable " << Variable << ": " << strerror(errno) << std::endl;
	}
}

const boost::filesystem::path get_home_directory()
{
	static boost::filesystem::path home;

#ifdef K3D_API_WIN32

	if(home.empty())
		home = boost::filesystem::path(string_t(getenv("APPDATA")));

	if(home.empty())
		home = boost::filesystem::path(string_t(getenv("USERPROFILE")));

#endif // K3D_API_WIN32

	if(home.empty())
		home = boost::filesystem::path(string_t(getenv("HOME")));

	if(home.empty())
	{
		home = boost::filesystem::path(string_t(DEFAULT_HOME_DIRECTORY));
		log() << warning << "Using default home directory [" << home.native() << "]" << std::endl;
	}

	return home;
}

const boost::filesystem::path get_options_directory()
{
	return get_home_directory() / boost::filesystem::path(".k3d");
}

const boost::filesystem::path get_temp_directory()
{
	static boost::filesystem::path temp_directory;

	if(temp_directory.empty())
	{
		if(!getenv("TMPDIR").empty())
		{
			temp_directory = boost::filesystem::path(string_t(getenv("TMPDIR")));
		}
		else if(!getenv("TMP").empty())
		{
			temp_directory = boost::filesystem::path(string_t(getenv("TMP")));
		}
		else if(!getenv("TEMP").empty())
		{
			temp_directory = boost::filesystem::path(string_t(getenv("TEMP")));
		}
		else
		{
			temp_directory = boost::filesystem::path(string_t(DEFAULT_TEMP_DIRECTORY));
		}
	}

	return temp_directory;
}

const boost::filesystem::path find_executable(const string_t& Executable)
{
	const string_t executable_name = k3d::system::executable_name(Executable);

	boost::filesystem::path result;

	std::vector<string_t> paths;
	boost::algorithm::split(paths, system::getenv("PATH"), boost::algorithm::is_any_of(":"));

	for(const string_t path_str : paths)
	{
		const boost::filesystem::path test_path = boost::filesystem::path(path_str) / boost::filesystem::path(executable_name);
		if(boost::filesystem::exists(test_path))
		{
			result = test_path;
			break;
		}
	}

	return result;
}

const boost::filesystem::path generate_temp_file()
{
#ifdef K3D_API_WIN32

	string_t buffer(MAX_PATH, '\0');
	return_val_if_fail(GetTempFileName(get_temp_directory().native().c_str(), "k3d", 0, const_cast<char*>(buffer.c_str())), boost::filesystem::path());
	buffer.resize(strlen(buffer.c_str()));

	return boost::filesystem::path(string_t(buffer));

#else // K3D_API_WIN32

	string_t buffer = (get_temp_directory() / boost::filesystem::path("k3d-XXXXXX")).native();
	int fd = mkstemp(const_cast<char*>(buffer.c_str()));
	return_val_if_fail(fd, boost::filesystem::path());
	close(fd);

	return boost::filesystem::path(string_t(buffer));

#endif // !K3D_API_WIN32
}

bool file_modification_time(const boost::filesystem::path& File, time_t& ModificationTime)
{
	struct stat statistics;
	if(-1 == stat(File.native().c_str(), &statistics))
		return false;

	ModificationTime = statistics.st_mtime;

	return true;
}

bool spawn(const boost::filesystem::path& Command, const std::vector<string_t>& Arguments, const spawn_type SpawnType, string_t& StandardOut, string_t& StandardError, const boost::filesystem::path& WorkingDirectory, const environment_t& Environment)
{
#ifdef K3D_API_WIN32
	assert_not_implemented();
#endif

	// Process the arguments
	detail::argv_from_vector args(Command.native(), Arguments);

	// Pipes to capture stdin and stderr
	int std_out_fd[2];
	int std_err_fd[2];

	pipe(std_out_fd);
	pipe(std_err_fd);

	pid_t child_pid = fork();

	if(child_pid == -1)
	{
		k3d::log() << error << "fork() failed with error " << errno << " (" << strerror(errno) << ") when executing " << Command.native() << std::endl;
		return false;
	}

	if(child_pid == 0) // child code
	{
		// Duplicate to stdout and stderr
		if(dup2(std_out_fd[1], STDOUT_FILENO) < 0)
		{
			k3d::log() << error << "dup2() failed with error " << errno << " (" << strerror(errno) << ") on stdout" << std::endl;
			exit(254);
		}
		if(dup2(std_err_fd[1], STDERR_FILENO) < 0)
		{
			k3d::log() << error << "dup2() failed with error " << errno << " (" << strerror(errno) << ") on stderr" << std::endl;
			exit(254);
		}

		for(const auto& env_entry : Environment)
		{
			setenv(env_entry.first, env_entry.second);
		}

		// Close pipes
		close(std_out_fd[0]);
		close(std_err_fd[0]);
		close(std_out_fd[1]);
		close(std_err_fd[1]);

		if(!WorkingDirectory.empty())
		{
			if(chdir(WorkingDirectory.native().c_str()) == -1)
			{
				k3d::log() << error << "chdir failed with error " << errno << " (" << strerror(errno) << ") when changing to working dir " << WorkingDirectory.native() << std::endl;
				exit(255);
			}
		}

		if(execvp(Command.native().c_str(), args.argv) == -1)
		{
			k3d::log() << error << "execvp failed with error " << errno << " (" << strerror(errno) << ") when executing " << Command.native() << std::endl;
			exit(255);
		}
	}
	else // parent code
	{
		// Close output sides of pipe
		close(std_out_fd[1]);
		close(std_err_fd[1]);

		if(SpawnType == SPAWN_SYNCHRONOUS)
		{
			const size_t buf_size = 1024;
			char std_buffer[buf_size];
			char err_buffer[buf_size];

			ssize_t nb_read_std = 0;
			ssize_t nb_read_err = 0;

			do
			{
				nb_read_std = read(std_out_fd[0], std_buffer, buf_size);
				nb_read_err = read(std_err_fd[0], err_buffer, buf_size);

				if(nb_read_std < 0)
				{
					k3d::log() << error << "Error " << errno << " reading spawned command stdout pipe: " << strerror(errno) << std::endl;
					return false;
				}
				if(nb_read_err < 0)
				{
					k3d::log() << error << "Error " << errno << " reading spawned command stderr pipe: " << strerror(errno) << std::endl;
					return false;
				}

				StandardOut.append(std_buffer, nb_read_std);
				StandardError.append(err_buffer, nb_read_err);
			} while(nb_read_err > 0 && nb_read_std > 0);

			int child_status;
			if(waitpid(child_pid, &child_status, 0) < 0)
			{
				k3d::log() << error << "Error " << errno << " while waiting for child to exit: " << strerror(errno) << std::endl;
			}

			if(child_status != 0)
			{
				k3d::log() << error << "Child process " << Command.native() << " failed with status " << WEXITSTATUS(child_status) << " and output: " << StandardError << std::endl;
				return false;
			}
		}
		else
		{
			assert_not_implemented();
		}
	}

	return true;
}

const paths_t decompose_path_list(const string_t Input)
{
	paths_t results;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> separator(SEARCHPATH_SEPARATOR_STRING);
	tokenizer input(Input, separator);

	for(tokenizer::iterator path = input.begin(); path != input.end(); ++path)
	{
		const string_t path_string = k3d::trim(*path);
		if(!path_string.empty())
			results.push_back(boost::filesystem::path(string_t(path_string)));
	}

	return results;
}

#ifdef K3D_API_WIN32

const string_t executable_name(const string_t& Executable)
{
	return Executable + ".exe";
}

#else // K3D_API_WIN32

const string_t executable_name(const string_t& Executable)
{
	return Executable;
}

#endif // !K3D_API_WIN32

#ifdef K3D_API_WIN32

void sleep(const double Seconds)
{
	if(Seconds <= 0.0)
		return;

	Sleep(static_cast<DWORD>(Seconds * 1000));
}

#else // K3D_API_WIN32

void sleep(const double Seconds)
{
	if(Seconds <= 0.0)
		return;

	timespec t;
	t.tv_sec = static_cast<long>(std::floor(Seconds));
	t.tv_nsec = static_cast<long>((Seconds - std::floor(Seconds)) * 1000000000);

	nanosleep(&t, 0);
}

#endif // !K3D_API_WIN32

} // namespace system

} // namespace k3d

