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

#include <boost/python.hpp>

#include <k3dsdk/python/filesystem_python.h>

#include <boost/filesystem/path.hpp>
#include <k3dsdk/types.h>

#include <boost/python/detail/api_placeholder.hpp>
using namespace boost::python;

namespace k3d
{

namespace python
{

const k3d::string_t root_name(const boost::filesystem::path& Self)
{
	return Self.root_name().native();
}

const k3d::string_t root_directory(const boost::filesystem::path& Self)
{
	return Self.root_directory().native();
}

const k3d::string_t leaf(const boost::filesystem::path& Self)
{
	return Self.leaf().native();
}

const k3d::string_t path_string(const boost::filesystem::path& Self)
{
	return Self.native();
}

class filesystem
{
public:
	static const boost::filesystem::path path(const k3d::string_t& GenericPath)
	{
		return boost::filesystem::path(GenericPath);
	}
};

void define_namespace_filesystem()
{
	scope outer = class_<filesystem>("filesystem", no_init)
		.def("path", filesystem::path)
		.staticmethod("path")
		;

	class_<boost::filesystem::path>("path",
		"Stores a filesystem path")
		.def(self == self)
		.def(self != self)
		.def(self / self)
		.def("root_path", &boost::filesystem::path::root_path)
		.def("root_name", root_name)
		.def("root_directory", root_directory)
		.def("leaf", leaf)
		.def("branch_path", &boost::filesystem::path::branch_path)
		.def("empty", &boost::filesystem::path::empty)
		.def("is_complete", &boost::filesystem::path::is_complete)
		.def("__str__", path_string)
		;

}

} // namespace python

} // namespace k3d

