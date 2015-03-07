// K-3D
// Copyright (c) 1995-2015, K-3D
//
// Contact: bart@bartjanssens.org
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

#include <k3d-i18n-config.h>

#include <k3dsdk/data.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/file_helpers.h>
#include <k3dsdk/gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/options.h>
#include <k3dsdk/value_demand_storage.h>

#include <k3dsdk/gl/ishader.h>

#include <boost/filesystem/path.hpp>

namespace module
{

namespace opengl
{

/// Encapsulates an OpenGL shader
class shader :
	public k3d::gl::ishader,
	public k3d::node
{
	typedef node base;

public:
	/// Return the current ID of the compiled shader, or 0 if something is wrong
	GLuint shader_id()
	{
		return m_shader_object.pipeline_value();
	}

	/// Request deletion of the shader, if it is no longer used
	void request_delete()
	{
		if(glIsProgram(m_cached_shader))
			glDeleteProgram(m_cached_shader);
	}

	/// Connect to the changed signal that indicates the shader ID has changed
	boost::signals2::connection connect_shader_changed_signal(const typename k3d::data::change_signal<GLuint>::changed_signal_t::slot_type& Slot)
	{
		return m_shader_object.changed_signal().connect(Slot);
	}

	GLenum type() const
	{
		return m_type;
	}

protected:
	shader(k3d::iplugin_factory& Factory, k3d::idocument& Document, const GLenum ShaderType) :
		base(Factory, Document),
		m_type(ShaderType),
		m_shader_path(init_owner(*this) + init_name("shader_path") + init_label(_("Shader Path")) + init_description(_("Shader Path")) + init_value(boost::filesystem::path()) + init_path_mode(k3d::ipath_property::READ) + init_path_type(k3d::options::path::opengl_shaders())),
		m_shader_object(init_owner(*this) + init_name("shader_object") + init_label(_("Shader Object")) + init_description(_("OpenGL ID for the shader object")) + init_value(0)),
		m_cached_shader(0)
	{
		m_shader_object.set_update_slot(boost::bind(&shader::execute, this, _1, _2));
		m_shader_path.changed_signal().connect(m_shader_object.make_slot());
	}

	virtual ~shader()
	{
		request_delete();
	}

private:
	void execute(const std::vector<k3d::ihint*>&, GLuint& Output)
	{

		if(glIsProgram(Output))
			glDeleteProgram(Output);

		const boost::filesystem::path shader_path = m_shader_path.pipeline_value();
		const std::string shader_code = k3d::read_file(shader_path);
		const char* code_cstr = shader_code.c_str();
		k3d::log() << debug << "Compiling shader " << shader_path.native() << std::endl;
		Output = glCreateShaderProgramv(m_type, 1, &code_cstr);
		if(Output == 0)
		{
			k3d::log() << "Error creating shader (0 was returned by glCreateShader)" << std::endl;
			return;
		}

		// Check status
		GLint result = GL_FALSE;
		glGetProgramiv(Output, GL_LINK_STATUS, &result);
		if(result == GL_FALSE)
		{
			GLint log_size;
			glGetProgramiv(Output, GL_INFO_LOG_LENGTH, &log_size);
			std::vector<GLchar> error_log(log_size);
			glGetProgramInfoLog(Output, log_size, &log_size, &error_log[0]);
			k3d::log() << error << "Shader compilation failed with message:\n" << error_log.data() << std::flush;

			glDeleteProgram(Output);
			Output = 0;
			return;
		}
		m_cached_shader = Output;
	}

	const GLenum m_type;
	k3d_data(boost::filesystem::path, immutable_name, explicit_change_signal, with_undo, local_storage, no_constraint, path_property, path_serialization) m_shader_path;
	k3d_data(GLuint, immutable_name, change_signal, no_undo, value_demand_storage, no_constraint, read_only_property, no_serialization) m_shader_object;
	GLuint m_cached_shader; // Copy that is accessible without triggering an update
};

class vertex_shader : public shader
{
	typedef shader base;
public:
	vertex_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base(Factory, Document, GL_VERTEX_SHADER)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<vertex_shader,
			k3d::interface_list<k3d::gl::ishader> >factory(
			k3d::uuid("4f847096-88ec-48d5-bbfb-30fb694f83fa"),
			"OpenGLVertexShader",
			_("An OpenGL vertex shader"),
			"OpenGL",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

k3d::iplugin_factory& vertex_shader_factory()
{
	return vertex_shader::get_factory();
}

class fragment_shader : public shader
{
	typedef shader base;
public:
	fragment_shader(k3d::iplugin_factory& Factory, k3d::idocument& Document) : base(Factory, Document, GL_FRAGMENT_SHADER)
	{
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<fragment_shader,
			k3d::interface_list<k3d::gl::ishader> >factory(
			k3d::uuid("306f6d2f-d6fb-433f-985c-bef9321306ed"),
			"OpenGLFragmentShader",
			_("An OpenGL fragment shader"),
			"OpenGL",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}
};

k3d::iplugin_factory& fragment_shader_factory()
{
	return fragment_shader::get_factory();
}

} // namespace opengl

} // namespace module
