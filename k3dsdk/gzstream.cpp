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

#include <k3dsdk/gzstream.h>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>

namespace k3d
{

namespace filesystem
{

igzstream::igzstream()
{
}

igzstream::igzstream(const boost::filesystem::path& name)
{
	open(name);
}

void igzstream::open(const boost::filesystem::path& name)
{
	reset();
	push(boost::iostreams::gzip_decompressor());
	push(boost::iostreams::file_source(name.generic_string()));
}

ogzstream::ogzstream()
{
}

ogzstream::ogzstream(const boost::filesystem::path& name)
{
	open(name);
}

void ogzstream::open(const boost::filesystem::path &name)
{
	reset();
	push(boost::iostreams::gzip_compressor());
	push(boost::iostreams::file_sink(name.generic_string()));
}

void ogzstream::close()
{
	reset();
}

} // namespace filesystem

} // namespace k3d

