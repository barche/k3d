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
#include <k3dsdk/share.h>
#include <k3dsdk/system.h>

#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/dynamic_gui_root.h>
#include <k3dsdk/qtui/utility.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <QQmlProperty>

k3d::qtui::qml_type_registrator<k3d::qtui::dynamic_gui_root> register_dynamic_gui_root;

namespace k3d
{

namespace qtui
{

namespace detail
{

void write_anchors(std::ostream& Stream, const k3d::string_t& Prefix, QQuickItem* Item, QQmlContext* Context)
{
	return_if_fail(Context != nullptr);

	QQmlProperty anchors_prop(Item, "anchors", Context);
	QObject* anchors = anchors_prop.read().value<QObject*>();

	std::vector<QString> property_names = {"fill"};

	const k3d::string_t anchors_prefix = Prefix + "anchors.";

	for(const QString& prop_name : property_names)
	{
		QQmlProperty prop(anchors, prop_name, Context);
		QObject* val = prop.read().value<QObject*>();
		if(val != nullptr)
		{
			k3d::string_t id = k3d::convert<k3d::string_t>(Context->nameForObject(val));
			if(id.empty())
			{
				if(val == Item->parentItem())
				{
					id = "parent";
				}
			}
			if(!id.empty())
			{
				Stream << anchors_prefix << k3d::convert<k3d::string_t>(prop_name) << ": " << id << "\n";
			}
			else
			{
				k3d::log() << error << "Failed to look up anchors.fill value, skipping" << std::endl;
			}
		}

	}
}

void write_layout(std::ostream& Stream, const k3d::string_t& Prefix, QQuickItem* Item, QQmlContext* Context)
{
	// Layout Properties we want to write, together with their defaults
	std::map<k3d::string_t, k3d::string_t> properties_to_save = {{"Layout.maximumWidth", "inf"}, {"Layout.minimumWidth", "0"}, {"Layout.fillWidth", "false"}};

	for(const auto& prop_pair : properties_to_save)
	{
		QQmlProperty prop(Item, k3d::convert<QString>(prop_pair.first), Context);
		const k3d::string_t value = k3d::convert<k3d::string_t>(prop.read().toString());
		if(value != prop_pair.second) // Only save if different from defaut
		{
			Stream << Prefix << prop_pair.first << ": " << value << "\n";
		}
	}
}

struct output_qml
{
	output_qml(std::ostream& Stream, const k3d::uint_t Level = 0) : m_stream(Stream), m_level(Level), m_prefix(Level*2, ' ')
	{
	}

	void operator()(igui_element& Elem) const
	{
		m_stream << "\n" << m_prefix << Elem.name() << " {\n";
		const std::string app_prefix((m_level+1)*2, ' ');

		QQuickItem* item = dynamic_cast<QQuickItem*>(&Elem);
		QQmlContext* ctx = qmlContext(item);
		return_if_fail(ctx != nullptr);

		const k3d::string_t id = k3d::convert<k3d::string_t>(ctx->nameForObject(item));
		if(!id.empty())
			m_stream << app_prefix << "id: " << id << "\n";

		write_anchors(m_stream, app_prefix, item, ctx);
		write_layout(m_stream, app_prefix, item, ctx);
		Elem.for_each_attribute([&](const k3d::string_t& key, const boost::any& value)
		{
			m_stream << app_prefix << key << ": " << property_value_to_string(boost::any_cast<QQmlProperty>(value)) << "\n";
		});
		Elem.for_each_child(output_qml(m_stream, m_level+1));
		m_stream << m_prefix << "}\n";
	}

	std::ostream& m_stream;
	const k3d::uint_t m_level;
	const k3d::string_t m_prefix;
};

}

dynamic_gui_root::dynamic_gui_root(QQuickItem *parent) : QQuickItem(parent)
{
}

dynamic_gui_root::~dynamic_gui_root()
{
	std::cout << "destroying root" << std::endl;
}

void dynamic_gui_root::set_config_file(const QUrl& ConfigFile)
{
	QUrl file_to_load = boost::filesystem::exists(k3d::convert<boost::filesystem::path>(ConfigFile)) ? ConfigFile : k3d::convert<QUrl>(k3d::share_path() / "qtui" / "qml" / "document_window.qml");
	m_config_file = ConfigFile;
	if(load_qml(file_to_load, this) == nullptr)
	{
		k3d::log() << error << "Failed to load Qml file: " << file_to_load.toString().toStdString() << std::endl;
	}

	clear_children();
	for(QQuickItem* child_item : childItems())
	{
		igui_element* child_elem = dynamic_cast<igui_element*>(child_item);
		if(child_elem != nullptr)
		{
			register_child(child_elem);
		}
	}

	k3d::log() << debug << "Loaded GUI config file " << k3d::convert<k3d::string_t>(file_to_load.toString()) << std::endl;

	emit config_file_changed(m_config_file);
}

void dynamic_gui_root::save_config()
{
	boost::filesystem::ofstream stream(k3d::convert<boost::filesystem::path>(m_config_file));

	// Read in the default header
	boost::filesystem::ifstream header_stream(k3d::share_path() / "qtui" / "qml" / "standard_header.qml");
	k3d::string_t header_line;
	while (std::getline(header_stream,header_line))
	{
		stream << header_line << "\n";
	}

	for_each_child(detail::output_qml(stream));
	stream.close();
	k3d::log() << debug << "Wrote GUI config to " << k3d::convert<k3d::string_t>(m_config_file.toLocalFile()) << std::endl;
}


} // namespace qtui

} // namespace k3d

