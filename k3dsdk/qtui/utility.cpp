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


#include <k3dsdk/log.h>
#include <k3dsdk/log_control.h>
#include <k3dsdk/result.h>

#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/gui_element.h>
#include <k3dsdk/qtui/utility.h>


#include <QQmlComponent>
#include <QQmlProperty>
#include <QQuickItem>

#include <set>

namespace k3d
{

namespace qtui
{

struct qml_registry
{
	typedef std::map<k3d::string_t, QUrl> templates_t;
	static templates_t& templates()
	{
		static templates_t templates_instance;
		return templates_instance;
	}

	typedef std::set<k3d::string_t> user_types_t;
	static user_types_t& user_types()
	{
		static user_types_t instance;
		return instance;
	}
};

QQuickItem* load_qml(const QUrl& QmlFile, QQuickItem* Parent)
{
	QQmlComponent component(qmlEngine(Parent), QmlFile);
	if(component.isError())
	{
		qWarning() << component.errors();
		return nullptr;
	}
	QQuickItem *loaded_item = qobject_cast<QQuickItem*>(component.create(qmlContext(Parent)));
	loaded_item->setParentItem(Parent);

	return loaded_item;
}

void register_qml_template(const k3d::string_t &Name, const QUrl TemplateUrl, const bool UserInstantiable)
{
	return_if_fail(qml_registry::templates().count(Name) == 0);
	qml_registry::templates()[Name] = TemplateUrl;
	if(UserInstantiable)
	{
		qml_registry::user_types().insert(Name);
	}
}

QUrl get_template_url(const k3d::string_t &Name)
{
	const qml_registry::templates_t::const_iterator found_it = qml_registry::templates().find(Name);
	if(found_it != qml_registry::templates().end())
		return found_it->second;

	return QUrl();
}

QStringList registered_types()
{
	QStringList result;
	for(const auto& registered_type : qml_registry::user_types())
	{
		result.append(k3d::convert<QString>(registered_type));
	}

	return result;
}

const char* qml_type_name(const QMetaObject& MetaObject)
{
	const int idx = MetaObject.indexOfClassInfo(qml_type_name_key);
	if(idx == -1)
	{
		k3d::log() << error << "no qml type name found for class " << MetaObject.className();
		return "";
	}

	return MetaObject.classInfo(idx).value();
}

k3d::string_t property_value_to_string(const QQmlProperty& Property)
{
	if(Property.property().isEnumType())
	{
		return k3d::string_t(Property.property().enumerator().scope()) + k3d::string_t(".") + k3d::string_t(Property.property().enumerator().valueToKey(Property.read().toInt()));
	}

	return k3d::convert<k3d::string_t>(Property.read().toString());
}

} // namespace qtui

} // namespace k3d

