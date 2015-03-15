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


#include <k3dsdk/qtui/glew_context.h>
#include <k3dsdk/qtui/viewport.h>

#include <QOpenGLFramebufferObject>
#include <QSGNode>
#include <QSGSimpleTextureNode>

namespace k3d
{

namespace qtui
{

class fbo_renderer : public QQuickFramebufferObject::Renderer
{
public:
	fbo_renderer(const node_wrapper& State) : m_state(State)
	{
		k3d::log() << debug << "creating new renderer" << std::endl;
	}

	void render()
	{
		k3d::iviewport_state* state = extract_node<k3d::iviewport_state>(m_state, "state");
		if(state == nullptr)
		{
			k3d::log() << error << "state is null, not rendering" << std::endl;
			return;
		}

		k3d::property::set_internal_value(state->pixel_width_property(), static_cast<k3d::uint_t>(framebufferObject()->size().width()));
		k3d::property::set_internal_value(state->pixel_height_property(), static_cast<k3d::uint_t>(framebufferObject()->size().height()));

		k3d::gl::irender_viewport* engine = dynamic_cast<k3d::gl::irender_viewport*>(k3d::property::pipeline_value<k3d::inode*>(state->render_engine_property()));
		if(engine == nullptr)
		{
			k3d::log() << error << "gl_engine is null, not rendering" << std::endl;
			return;
		}

		k3d::icamera* camera = dynamic_cast<k3d::icamera*>(k3d::property::pipeline_value<k3d::inode*>(state->camera_property()));
		if(camera == nullptr)
		{
			k3d::log() << error << "camera is null, not rendering" << std::endl;
			return;
		}

		m_context.begin();
		engine->render_viewport(*state);
		m_context.end();

		update();
	}

	QOpenGLFramebufferObject *createFramebufferObject(const QSize &size)
	{
		QOpenGLFramebufferObjectFormat format;
		format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
		format.setSamples(4);
		return new QOpenGLFramebufferObject(size, format);
	}

private:
	node_wrapper m_state;
	glew_context m_context;
};

void viewport::set_state(const node_wrapper& State)
{
	m_state = State;
	emit state_changed(m_state);
}

QQuickFramebufferObject::Renderer *viewport::createRenderer() const
{
	return new fbo_renderer(m_state);
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

} // namespace qtui

} // namespace k3d

