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

// This code leans heavily on boost::filesystem::path, most changes are to support ustring UTF-8 storage

#include <k3dsdk/path.h>
#include <k3dsdk/result.h>

namespace k3d
{

namespace filesystem
{

/////////////////////////////////////////////////////////////////////////////////////////////
// make_relative_path

boost::filesystem::path make_relative_path(const boost::filesystem::path& AbsolutePath, const boost::filesystem::path& ReferencePath)
{
	// The AbsolutePath must actually *be* an absolute path!
	return_val_if_fail(AbsolutePath.is_complete(), boost::filesystem::path());

	// As a special-case, if the AbsolutePath and ReferencePath don't share the same root name, return the AbsolutePath (which is the best we can do)
	if(AbsolutePath.root_name() != ReferencePath.root_name())
		return AbsolutePath;

	boost::filesystem::path relative_path;

	const boost::filesystem::path root_path = ReferencePath;
	const boost::filesystem::path absolute_path(AbsolutePath);

	boost::filesystem::path::iterator a = root_path.begin();
	boost::filesystem::path::iterator b = absolute_path.begin();
	while(a != root_path.end() && b != absolute_path.end() && *a == *b)
	{
		++a;
		++b;
	}

	for(; a != root_path.end(); ++a)
		relative_path /= boost::filesystem::path("..");
	for(; b != absolute_path.end(); ++b)
		relative_path /= boost::filesystem::path(*b);

	return relative_path;
}

} // namespace filesystem

} // namespace k3d

