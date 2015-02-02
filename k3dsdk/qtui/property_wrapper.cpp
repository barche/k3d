// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
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

#include <k3d-i18n-config.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/imesh_painter_gl.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/property.h>
#include <k3dsdk/transform.h>

#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/property_wrapper.h>

namespace k3d
{

namespace qtui
{

property_wrapper::property_wrapper(iproperty& Property, QObject* Parent) :
	QObject(Parent),
	m_property(Property)
{
	m_property.property_changed_signal().connect(boost::bind(&property_wrapper::on_value_changed, this, _1));
}

QVariant property_wrapper::value() const
{
	return k3d::convert<QVariant>(m_property.property_pipeline_value());
}

void property_wrapper::set_value(const QVariant &Value)
{
	iwritable_property* writable_property = dynamic_cast<iwritable_property*>(&m_property);
	return_if_fail(writable_property);
	writable_property->property_set_value(k3d::convert<boost::any>(Value));
}

void property_wrapper::on_value_changed(ihint *)
{
	emit value_changed(value());
}


} // namespace qtui

} // namespace k3d

