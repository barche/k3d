#ifndef K3DSDK_PATH_H
#define K3DSDK_PATH_H

// K-3D
// Copyright (c) 1995-2007, Timothy M. Shead
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

#include <k3dsdk/types.h>

#include <boost/filesystem/path.hpp>
#include <vector>

namespace k3d
{

namespace filesystem
{

/// Defines a collection of paths
typedef std::vector<boost::filesystem::path> path_list;

/// Converts the given absolute path into a relative path, relative to the given reference path
boost::filesystem::path make_relative_path(const boost::filesystem::path& AbsolutePath, const boost::filesystem::path& ReferencePath);

} // namespace filesystem

} // namespace k3d

#endif // !K3DSDK_PATH_H

