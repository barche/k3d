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

#include <k3dsdk/irender_viewport_gl.h>
#include <k3dsdk/icamera.h>

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
	fbo_renderer(const node_wrapper& GlEngine, const node_wrapper& Camera) : m_gl_engine(GlEngine), m_camera(Camera)
	{
	}

	void render()
	{
		k3d::gl::irender_viewport* engine = extract_node<k3d::gl::irender_viewport>(m_gl_engine, "gl_engine");
		if(engine == nullptr)
			k3d::log() << error << "gl_engine is null, not rendering" << std::endl;

		k3d::icamera* camera = extract_node<k3d::icamera>(m_camera, "camera");
		if(camera == nullptr)
			k3d::log() << error << "camera is null, not rendering" << std::endl;

		GLdouble view_matrix[16];
		GLdouble projection_matrix[16];
		GLint viewport[4];

		m_context.begin();
		engine->render_viewport(*camera, framebufferObject()->size().width(), framebufferObject()->size().height(), view_matrix, projection_matrix, viewport);
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
	node_wrapper m_gl_engine;
	node_wrapper m_camera;
	glew_context m_context;

	//LogoRenderer m_logo;
};

void viewport::set_gl_engine(const node_wrapper& GlEngine)
{
	m_gl_engine = GlEngine;
	emit gl_engine_changed(m_gl_engine);
}

void viewport::set_camera(const node_wrapper& Camera)
{
	m_camera = Camera;
	emit camera_changed(m_camera);
}

QQuickFramebufferObject::Renderer *viewport::createRenderer() const
{
	return new fbo_renderer(m_gl_engine, m_camera);
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

