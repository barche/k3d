#ifndef K3DSDK_QTUI_VIEWPORT_H
#define K3DSDK_QTUI_VIEWPORT_H

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

#include <k3dsdk/qtui/node_wrapper.h>

#include <QQuickFramebufferObject>

namespace k3d
{

namespace qtui
{

/////////////////////////////////////////////////////////////////////////////
// viewport

/// The viewport widget for the Qt interface
class viewport :
	public QQuickFramebufferObject
{
	Q_OBJECT
	Q_PROPERTY(node_wrapper gl_engine READ gl_engine WRITE set_gl_engine NOTIFY gl_engine_changed)
	Q_PROPERTY(node_wrapper camera READ camera WRITE set_camera NOTIFY camera_changed)
public:
	Renderer *createRenderer() const;

	/// gl_engine property implementation
	const node_wrapper& gl_engine() { return m_gl_engine; }
	void set_gl_engine(const node_wrapper& GlEngine);

	/// camera property implementation
	const node_wrapper& camera() { return m_camera; }
	void set_camera(const node_wrapper& Camera);

signals:
	void gl_engine_changed(node_wrapper);
	void camera_changed(node_wrapper);

protected:
	QSGNode* updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData);

private:
	node_wrapper m_gl_engine = nullptr;
	node_wrapper m_camera = nullptr;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_VIEWPORT_H

