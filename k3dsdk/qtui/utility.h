#ifndef K3DSDK_QTUI_UTILITY_H
#define K3DSDK_QTUI_UTILITY_H

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
	\author Bart Janssens
*/

#include <k3dsdk/result.h>
#include <k3dsdk/types.h>

#include <QStringList>
#include <QtQml>
#include <QUrl>

class QQuickItem;
class QQmlProperty;

namespace k3d
{

namespace qtui
{

/// Load the given QmlFile and return a pointer to the top-level Item
QQuickItem* load_qml(const QUrl& QmlFile, QQuickItem* Parent);

/// Register the template corresponding to a Qml class
void register_qml_template(const k3d::string_t& Name, const QUrl TemplateUrl, const bool UserInstantiable);

/// Get the URL for the QML template associated with the given K-3D Qml type
QUrl get_template_url(const k3d::string_t& Name);

/// List of the registered types
QStringList registered_types();

const char* qml_type_name(const QMetaObject& MetaObject);

/// Helper to register a Qml type in its cpp file by simply declaring a global variable of this type
template<typename QmlType>
struct qml_type_registrator
{
	qml_type_registrator(const QUrl TemplateURL = QUrl(), const bool UserInstantiable = false)
	{
		const char* name = qml_type_name(QmlType::staticMetaObject);
		assert_error(name != "");
		register_qml_template(name, TemplateURL, UserInstantiable);
		qmlRegisterType<QmlType>("K3D", 1, 0, name);
	}
};

/// Register a meta type
template<typename QmlType>
struct qml_meta_type_registrator
{
	qml_meta_type_registrator(const k3d::string_t& Name)
	{
		qRegisterMetaType<QmlType>(Name.c_str());
	}
};

k3d::string_t property_value_to_string(const QQmlProperty& Property);

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_UTILITY_H

