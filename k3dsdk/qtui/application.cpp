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
*/

#include <k3dsdk/application.h>
#include <k3dsdk/iapplication.h>
#include <k3dsdk/idocument.h>
#include <k3dsdk/log.h>
#include <k3dsdk/result.h>

#include <k3dsdk/qtui/application.h>
#include <k3dsdk/qtui/convert.h>

#include <QtQml>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// application

application::application(int &argc, char **argv) : QApplication(argc, argv)
{
	QObject::connect(this, &QApplication::aboutToQuit, this, &application::on_about_to_quit);
}

QUrl application::qml_template(const QString &type)
{
	return get_template_url(convert<k3d::string_t>(type));
}

void application::on_new_document()
{
	idocument* document = k3d::application().create_document();
	return_if_fail(document);
}

void application::on_about_to_quit()
{
	k3d::log() << info << "K-3D is quitting" << std::endl;
	k3d::application().exit();
}

} // namespace qtui

} // namespace k3d

