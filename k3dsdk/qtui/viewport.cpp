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

#include <k3dsdk/icamera.h>
#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/iviewport_state.h>
#include <k3dsdk/property.h>
#include <k3dsdk/result.h>

#include <k3dsdk/qtui/events.h>
#include <k3dsdk/qtui/glew_context.h>
#include <k3dsdk/qtui/viewport.h>

#include <QOpenGLFramebufferObject>
#include <QSGNode>
#include <QSGSimpleTextureNode>

namespace k3d
{

namespace qtui
{

viewport::viewport(QQuickItem *parent) : QQuickFramebufferObject(parent)
{
	setAcceptedMouseButtons(Qt::AllButtons);
}

class viewport::fbo_renderer : public QQuickFramebufferObject::Renderer
{
public:
	fbo_renderer(k3d::iviewport_state* State) : m_state(State)
	{
		k3d::log() << debug << "creating new renderer" << std::endl;
	}

	void render()
	{
		if(m_state == nullptr)
		{
			k3d::log() << error << "state is null, not rendering" << std::endl;
			return;
		}

		k3d::property::set_internal_value(m_state->pixel_width_property(), static_cast<k3d::uint_t>(framebufferObject()->size().width()));
		k3d::property::set_internal_value(m_state->pixel_height_property(), static_cast<k3d::uint_t>(framebufferObject()->size().height()));

		k3d::gl::irender_viewport* engine = dynamic_cast<k3d::gl::irender_viewport*>(k3d::property::pipeline_value<k3d::inode*>(m_state->render_engine_property()));
		if(engine == nullptr)
		{
			k3d::log() << error << "gl_engine is null, not rendering" << std::endl;
			return;
		}

		k3d::icamera* camera = dynamic_cast<k3d::icamera*>(k3d::property::pipeline_value<k3d::inode*>(m_state->camera_property()));
		if(camera == nullptr)
		{
			k3d::log() << error << "camera is null, not rendering" << std::endl;
			return;
		}

		m_context.begin();
		engine->render_viewport(*m_state);
		m_context.end();
	}

	QOpenGLFramebufferObject *createFramebufferObject(const QSize &size)
	{
		QOpenGLFramebufferObjectFormat format;
		format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
		format.setSamples(4);
		return new QOpenGLFramebufferObject(size, format);
	}

private:
	k3d::iviewport_state* m_state;
	glew_context m_context;
};

void viewport::set_state(const node_wrapper& State)
{
	m_redraw_connection.disconnect();
	m_state = State;
	if(m_state.node() != nullptr)
	{
		k3d::gl::irender_viewport* engine = dynamic_cast<k3d::gl::irender_viewport*>(k3d::property::pipeline_value<k3d::inode*>(extract_node<k3d::iviewport_state>(m_state, "state")->render_engine_property()));
		engine->redraw_request_signal().connect(boost::bind(&viewport::on_redraw_request, this, _1));
	}
	emit state_changed(m_state);

}

QQuickFramebufferObject::Renderer *viewport::createRenderer() const
{
	return new fbo_renderer(extract_node<k3d::iviewport_state>(m_state, "state"));
}


QSGNode* viewport::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *nodeData)
{
	// This is needed to prevent the image from being upside-down
	if(!node)
	{
		node = QQuickFramebufferObject::updatePaintNode(node, nodeData);
		QSGSimpleTextureNode *n = static_cast<QSGSimpleTextureNode*>(node);
		if(n)
			n->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
		return node;
	}
	return QQuickFramebufferObject::updatePaintNode(node, nodeData);
}

void viewport::mousePressEvent(QMouseEvent *Event)
{
	process_mouse_event(Event);
}

void viewport::mouseReleaseEvent(QMouseEvent* Event)
{
	process_mouse_event(Event);
}

void viewport::mouseMoveEvent(QMouseEvent * Event)
{
	process_mouse_event(Event);
}

void viewport::process_mouse_event(QMouseEvent* Event)
{
	k3d::iviewport_state* state = extract_node<k3d::iviewport_state>(m_state, "state");
	k3d::input_event k3d_event = make_k3d_event(*Event);
	k3d_event.position[0] = Event->x();
	k3d_event.position[1] = Event->y();
	state->input_event_signal()(k3d_event);
}

void viewport::on_redraw_request(gl::irender_viewport::redraw_type_t RedrawType)
{
	update();
}

} // namespace qtui

} // namespace k3d

