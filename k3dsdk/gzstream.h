#ifndef K3DSDK_GZSTREAM_H
#define K3DSDK_GZSTREAM_H

// K-3D
// Copyright (c) 1995-2011, Timothy M. Shead
// Copyright (c) 2011-2015, Bart Janssens
//
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

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/filtering_stream.hpp>


namespace k3d
{

namespace filesystem
{

/// ifstream replacement that can read files with gzip compression
class igzstream :
	public boost::iostreams::filtering_istream
{
public:
	igzstream();
	igzstream(const boost::filesystem::path& name);

	void open(const boost::filesystem::path& name);
	void close();
};

/// ofstream replacement that writes files with gzip compression
class ogzstream :
	public boost::iostreams::filtering_ostream
{
public:
	ogzstream();
	ogzstream(const boost::filesystem::path& name);

	void open(const boost::filesystem::path& name);
	void close();
};

} // namespace filesystem

} // namespace k3d

#endif // !K3DSDK_GZSTREAM_H

