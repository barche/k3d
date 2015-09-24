#ifndef K3DSDK_QTUI_DYNAMIC_GUI_ROOT_H
#define K3DSDK_QTUI_DYNAMIC_GUI_ROOT_H

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

#include <k3dsdk/qtui/gui_element.h>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// dynamic_gui_root

/// Root for a set of K-3D widgets
class dynamic_gui_root :
	public QQuickItem, public gui_element
{
	Q_OBJECT
	Q_CLASSINFO("qml_type_name", "DynamicGuiRoot")
	Q_PROPERTY(QUrl config_file READ config_file WRITE set_config_file NOTIFY config_file_changed)
public:
	dynamic_gui_root(QQuickItem *parent = 0);
	~dynamic_gui_root();

	QUrl config_file() const
	{
		return m_config_file;
	}

	void set_config_file(const QUrl& ConfigFile);

	Q_INVOKABLE void save_config();

signals:
	void config_file_changed(QUrl);

private:
	QUrl m_config_file;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_DYNAMIC_GUI_ROOT_H

