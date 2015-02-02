#ifndef K3DSDK_QTUI_PROPERTY_WRAPPER_H
#define K3DSDK_QTUI_PROPERTY_WRAPPER_H

// K-3D
// Copyright (c) 1995-2015, K-3D
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
	\author Bart Janssens (bart@bartjanssens.org)
*/

#include <k3dsdk/iproperty.h>
#include <k3dsdk/iwritable_property.h>
#include <k3dsdk/result.h>

#include <QObject>
#include <QVariant>

namespace k3d
{

namespace qtui
{

/// Wrapper class for a property, exposing it to QML
class property_wrapper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QVariant value READ value WRITE set_value NOTIFY value_changed)
public:
	property_wrapper(iproperty& Property, QObject* Parent);

	QVariant value() const;

	void set_value(const QVariant& Value);

signals:
	void value_changed(const QVariant& Value);

protected:
	void on_value_changed(ihint*);

	iproperty& m_property;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_PROPERTY_WRAPPER_H

