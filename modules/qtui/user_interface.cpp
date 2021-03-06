// K-3D
// Copyright (c) 1995-2010, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your argument) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA	02111-1307	USA

/** \file
	\author Tim Shead (tshead@k-3d.com)
*/

#include "config.h"
#include "user_interface.h"

#include <k3d-i18n-config.h>
#include <k3d-version-config.h>
#include <k3dsdk/application.h>
#include <k3dsdk/application_plugin_factory.h>
#include <k3dsdk/classes.h>
#include <k3dsdk/high_res_timer.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument_importer.h>
#include <k3dsdk/iscripted_action.h>
#include <k3dsdk/iscript_engine.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/plugin.h>
#include <k3dsdk/share.h>

#include <k3dsdk/qtui/application.h>
#include <k3dsdk/qtui/convert.h>
#include <k3dsdk/qtui/document.h>

#include <QIcon>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QQuickItem>
#include <QSurfaceFormat>

#include <boost/scoped_ptr.hpp>

#include <iomanip>

namespace module
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// user_interface

void qt_message_output(QtMsgType Type, const QMessageLogContext& Context, const QString& Message)
{
	switch(Type)
	{
	case QtDebugMsg:
		k3d::log() << debug << k3d::convert<k3d::string_t>(Message) << std::endl;
		break;
	case QtWarningMsg:
		k3d::log() << warning << k3d::convert<k3d::string_t>(Message) << std::endl;
		break;
	case QtCriticalMsg:
		k3d::log() << error << k3d::convert<k3d::string_t>(Message) << std::endl;
		break;
	case QtFatalMsg:
		k3d::log() << critical << k3d::convert<k3d::string_t>(Message) << std::endl;
		abort();
	}
}

user_interface::user_interface()
{
	qInstallMessageHandler(qt_message_output);
	//QLoggingCategory::setFilterRules(QStringLiteral("qt.scenegraph.info=true\nqt.scenegraph.renderloop=true\nqt.quick.dialogs.window=true\nqt.quick.touch\nqt.quick.mouse\nqt.quick.focus\nqt.quick.dirty"));
}

user_interface::~user_interface()
{
	for(std::vector<k3d::iunknown*>::iterator plugin = m_auto_start_plugins.begin(); plugin != m_auto_start_plugins.end(); ++plugin)
	{
		if(k3d::iscripted_action* const scripted_action = dynamic_cast<k3d::iscripted_action*>(*plugin))
		{
			k3d::iscript_engine::context context;
			context["command"] = k3d::string_t("shutdown");
			scripted_action->execute(context);
		}
	}

	for(std::vector<k3d::iunknown*>::iterator plugin = m_auto_start_plugins.begin(); plugin != m_auto_start_plugins.end(); ++plugin)
		delete *plugin;

	m_auto_start_plugins.clear();
}

void user_interface::get_command_line_arguments(boost::program_options::options_description& Description)
{
	Description.add_options()
		("no-splash", "Disables the startup splash screen.")
		;
}

const k3d::ievent_loop::arguments_t user_interface::parse_startup_arguments(const arguments_t& Arguments, bool& Quit, bool& Error)
{
	// Keep track of whether to display a splash screen or not ...
	bool show_splash = true;

	// Qt expects to parse and modify argc / argv, so create some temporary storage for it to fiddle with ...
	static std::vector<char*> argv_buffer;
	argv_buffer.push_back(const_cast<char*>("k3d"));
	static int argc = argv_buffer.size();
	static char** argv = &argv_buffer[0];

	// We return any "unused" arguments ...
	arguments_t unused;

	// For each command-line argument ...
	for(arguments_t::const_iterator argument = Arguments.begin(); argument != Arguments.end(); ++argument)
	{
		if(argument->string_key == "no-splash")
		{
			show_splash = false;
		}
		else
		{
			unused.push_back(*argument);
		}
	}

	m_application.reset(new k3d::qtui::application(argc, argv));
	m_application->setApplicationName("K-3D");
	m_application->setApplicationVersion(K3D_VERSION);
	m_application->setOrganizationDomain("k-3d.org");
	m_application->setOrganizationName("k-3d.org");

	m_application->setWindowIcon(QIcon(":/QTUI/window_icon.png"));
	m_application->addLibraryPath(K3D_EXTRA_QT_PLUGINS);

	m_engine.reset(new QQmlEngine());
	m_engine->rootContext()->setContextProperty("k3d", m_application.get());

	const std::string qtui_default_interface = "qtui_default_interface";
	if(k3d::options::get_path(qtui_default_interface).empty())
	{
		k3d::options::set_path(qtui_default_interface, k3d::system::get_options_directory() / "default_document_window.qml");
	}

	m_engine->rootContext()->setContextProperty("default_document_window_qml", k3d::convert<QUrl>(k3d::options::get_path(qtui_default_interface)));

	QObject::connect(m_engine.get(), &QQmlEngine::quit, m_application.get(), &QApplication::quit);

	m_document_window_component.reset(new QQmlComponent(m_engine.get()));
	m_document_window_component->loadUrl(QUrl(QStringLiteral("qrc:/QTUI/document_window.qml")), QQmlComponent::PreferSynchronous);

	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setMajorVersion(4);
	format.setMinorVersion(3); // 4.3 for debugging output
	format.setOption(QSurfaceFormat::DebugContext);
	QSurfaceFormat::setDefaultFormat(format);

	k3d::log() << info << "Loading Qt plugins from " << k3d::convert<k3d::string_t>(m_application->libraryPaths().join(", ")) << std::endl;
	
//	if(show_splash)
//	{
//		m_splash_box.reset(new QSplashScreen(QPixmap(":/QTUI/splash.png")));
//		m_splash_box->show();
//		m_application->processEvents();
//	}

	return unused;
}

void user_interface::startup_message_handler(const k3d::string_t& Message)
{
//	if(m_splash_box.get())
//	{
//		m_splash_box->showMessage(Message.c_str());
//		m_application->processEvents();
//	}
}

void user_interface::display_user_interface()
{
	k3d::application().connect_document_created_signal(boost::bind(&user_interface::on_new_document, this, _1));
	m_application->on_new_document();

//	main_window* const window = new main_window(*document);
//	window->show();

//	m_splash_box.reset();

//	// Setup auto-start plugins ...
//	const k3d::plugin::factory::collection_t factories = k3d::plugin::factory::lookup("qtui:application-start", "true");
//	for(k3d::plugin::factory::collection_t::const_iterator factory = factories.begin(); factory != factories.end(); ++factory)
//	{
//		k3d::log() << info << "Creating plugin [" << (**factory).name() << "] via qtui:application-start" << std::endl;

//		k3d::iunknown* const plugin = k3d::plugin::create(**factory);
//		if(!plugin)
//		{
//			k3d::log() << error << "Error creating plugin [" << (**factory).name() << "] via qtui:application-start" << std::endl;
//			continue;
//		}
//		m_auto_start_plugins.push_back(plugin);

//		if(k3d::iscripted_action* const scripted_action = dynamic_cast<k3d::iscripted_action*>(plugin))
//		{
//			k3d::iscript_engine::context context;
//			context["command"] = k3d::string_t("startup");
//			scripted_action->execute(context);
//		}
//	}
}

const k3d::ievent_loop::arguments_t user_interface::parse_runtime_arguments(const arguments_t& Arguments, bool& Quit, bool& Error)
{
	return Arguments;
}

void user_interface::start_event_loop()
{
	m_application->exec();
}

void user_interface::stop_event_loop()
{
	m_application->quit();
}

void user_interface::open_uri(const k3d::string_t& URI)
{
	assert_not_reached();
}

void user_interface::message(const k3d::string_t& Message)
{
	//k3d::qtui::message(k3d::convert<QString>(Message), "");
}

void user_interface::warning_message(const k3d::string_t& Message)
{
	//k3d::qtui::warning_message(k3d::convert<QString>(Message), "");
}

void user_interface::error_message(const k3d::string_t& Message)
{
	//k3d::qtui::error_message(k3d::convert<QString>(Message), "");
}

k3d::uint_t user_interface::query_message(const k3d::string_t& Message, const k3d::uint_t DefaultOption, const std::vector<k3d::string_t>& Options)
{
	assert_not_implemented();
	return 0;
}

void user_interface::nag_message(const k3d::string_t& Type, const k3d::string_t& Message, const k3d::string_t& SecondaryMessage)
{
	//k3d::qtui::nag_message_dialog::nag(Type, Message, SecondaryMessage);
}

bool user_interface::get_file_path(const k3d::ipath_property::mode_t Mode, const k3d::string_t& Type, const k3d::string_t& Prompt, const boost::filesystem::path& OldPath, boost::filesystem::path& Result)
{
	assert_not_implemented();
	return false;
}

bool user_interface::show(iunknown& Object)
{
	assert_not_implemented();
	return false;
}

void user_interface::synchronize()
{
	assert_not_implemented();
}

boost::signals2::connection user_interface::get_timer(const double FrameRate, const k3d::void_signal_t::slot_type& Slot)
{
	assert_not_implemented();
	return boost::signals2::connection();
}

k3d::uint_t user_interface::watch_path(const boost::filesystem::path& Path, const k3d::void_signal_t::slot_type& Slot)
{
	assert_not_implemented();
	return 0;
}

void user_interface::unwatch_path(const k3d::uint_t WatchID)
{
	assert_not_implemented();
}

void user_interface::on_new_document(k3d::idocument& Document)
{
	k3d::qtui::populate_new_document(Document);
	build_document_window(Document);
}

void user_interface::build_document_window(k3d::idocument &Document)
{
	if(m_document_window_component->status() == QQmlComponent::Error)
	{
		k3d::log() << error << "document_window_component error: " << k3d::convert<k3d::string_t>(m_document_window_component->errors()[0].toString()) << std::endl;
	}

	QQmlContext* document_context = new QQmlContext(m_engine->rootContext());
	k3d::qtui::document_model* document_model = new k3d::qtui::document_model(Document);
	document_context->setContextProperty("document", document_model);
	document_model->setParent(document_context); // Ensures deletion


	QObject* document_window = m_document_window_component->create(document_context);
	document_context->setParent(document_window); // Ensures deletion
}

k3d::iplugin_factory& user_interface::get_factory()
{
	static k3d::application_plugin_factory<user_interface,
		k3d::interface_list<k3d::ievent_loop> > factory(
		k3d::uuid(0xc7c652c7, 0xfb4349fe, 0x9c7d52c4, 0x9f9e9d3f),
		"QtUI",
		"Qt User Interface (QtUI)",
		"Interface");

	return factory;
}

} // namespace qtui

} // namespace module

K3D_MODULE_START(Registry)
	Registry.register_factory(module::qtui::user_interface::get_factory());
K3D_MODULE_END

