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

#include <k3dsdk/algebra.h>
#include <k3dsdk/input_event.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/qtui/node_wrapper.h>
#include <k3dsdk/signal_system.h>

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
	Q_PROPERTY(node_wrapper state READ state WRITE set_state NOTIFY state_changed)
public:
	viewport(QQuickItem *parent = 0);

	Renderer *createRenderer() const;

	/// gl_engine property implementation
	const node_wrapper& state() { return m_state; }
	void set_state(const node_wrapper& State);

signals:
	void state_changed(node_wrapper);

protected:
	QSGNode* updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData);
	void mousePressEvent(QMouseEvent* Event);
	void mouseReleaseEvent(QMouseEvent* Event);
	void mouseMoveEvent(QMouseEvent * Event);

private:
	node_wrapper m_state = nullptr;

	// The last event that was captured, if any
	std::shared_ptr<k3d::input_event> m_last_event;
	// Position for the last event
	k3d::point2 m_last_event_position;

	class fbo_renderer;

	// Process a mouse event
	void process_mouse_event(QMouseEvent* Event);

	void on_redraw_request(k3d::gl::irender_viewport::redraw_type_t RedrawType);

	boost::signals2::connection m_redraw_connection;
};

} // namespace qtui

} // namespace k3d

#endif // !K3DSDK_QTUI_VIEWPORT_H

