#ifndef K3DSDK_QTUI_SPLIT_VIEW_H
#define K3DSDK_QTUI_SPLIT_VIEW_H

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

//Q_DECLARE_METATYPE(Qt::Orientation)

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// split_view

/// Root for a set of K-3D widgets
class split_view :
	public QQuickItem, public gui_element
{
	Q_OBJECT
	Q_CLASSINFO("qml_type_name", "SplitView")
	Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE set_orientation NOTIFY orientation_changed)
public:
	split_view(QQuickItem *parent = 0);
	~split_view();

	//Q_ENUMS(Qt::Orientation)

	Qt::Orientation orientation() const
	{
		return m_orientation;
	}

	void set_orientation(const Qt::Orientation Orientation);

signals:
	void orientation_changed(Qt::Orientation);

protected:
	virtual void componentComplete();
private:
	Qt::Orientation m_orientation = Qt::Horizontal;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_SPLIT_VIEW_H

