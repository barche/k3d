#ifndef K3DSDK_GL_PROGRAM_H
#define K3DSDK_GL_PROGRAM_H

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
#include <k3dsdk/gl.h>
#include <k3dsdk/node.h>
#include <k3dsdk/node_storage.h>
#include <k3dsdk/utility_gl.h>
#include <k3dsdk/value_demand_storage.h>

#include <k3dsdk/gl/ishader.h>
#include <k3dsdk/gl/iprogram.h>

namespace module
{

namespace opengl
{

class program;

namespace detail
{

/// Encapsulates a shader property, taking care of the changed signal
struct shader_property
{
	shader_property(program* Owner, const char* Name, const char* Label, const char* Description, const GLenum Type, const GLbitfield PipelineBit);

	~shader_property()
	{
		m_connection.disconnect();
	}

	void on_shader_node_changed(k3d::ihint* Hint)
	{
		k3d::gl::ishader* new_shader = m_shader.pipeline_value();

		if(new_shader == nullptr)
			return;

		if(new_shader->type() != m_type)
		{
			k3d::log() << error << "Incorrect shader type set in property " << m_shader.property_name() << std::endl;
			return;
		}

		m_connection = new_shader->connect_shader_changed_signal(boost::bind(&shader_property::on_shader_changed, this, _1));
		on_shader_changed(Hint);
	}

	void on_shader_node_about_to_change(k3d::gl::ishader* OldShader)
	{
		m_connection.disconnect();
		if(OldShader != nullptr)
		{
			OldShader->request_delete();
		}
	}

	void on_shader_changed(k3d::ihint*);

	GLuint shader_id()
	{
		return_val_if_fail(m_shader.pipeline_value(), 0);
		return m_shader.pipeline_value()->shader_id();
	}

	k3d_data(k3d::gl::ishader*, immutable_name, change_signal, with_undo, node_storage, no_constraint, node_property, node_serialization) m_shader;
	boost::signals2::connection m_connection;
	program* m_program;
	const GLenum m_type;
	const GLbitfield m_pipeline_bit;
};

/// Thrown when the shader is invalid
class invalid_shader_program : public std::runtime_error
{
public:
	invalid_shader_program(const std::string& Message) : std::runtime_error(Message)
	{
	}
};

} // namespace detail

/// Encapsulates an OpenGL program pipeline object
class program :
	public k3d::node,
	public k3d::gl::iprogram
{
	typedef k3d::node base;

public:
	program(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_program_id(0),
		m_vertex_shader(this, "vertex_shader", _("Vertex Shader"), _("Vertex Shader"), GL_VERTEX_SHADER, GL_VERTEX_SHADER_BIT),
		m_fragment_shader(this, "fragment_shader", _("Fragment Shader"), _("Fragment Shader"), GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER_BIT),
		m_need_use_stages(true),
		m_error(false)
	{
	}

	~program()
	{
		if(glIsProgramPipeline(m_program_id))
			glDeleteProgramPipelines(1, &m_program_id);
	}

	bool bind()
	{
		ensure_created();
		glUseProgram(0);

		if(m_need_use_stages)
		{
			m_need_use_stages = false;
			m_error = false;
			m_uniform_map.clear();
			try
			{
				use_program_stage(GL_VERTEX_SHADER_BIT, m_vertex_shader.shader_id());
				use_program_stage(GL_FRAGMENT_SHADER_BIT, m_fragment_shader.shader_id());
			}
			catch(const detail::invalid_shader_program&)
			{
				k3d::log() << error << "Invalid shaders detected for program [" << name() << "], aborting pipeline bind" << std::endl;
				m_error = true;
			}
		}

		if(m_error)
			return false;

		glBindProgramPipeline(m_program_id);
		return_val_if_fail(glIsProgramPipeline(m_program_id), false);
		return true;
	}

	void set_uniform(const std::string& Name, const k3d::matrix4 &Matrix)
	{
		const loc_t loc = uniform_loc(Name);
		GLfloat gl_mat[16];
    Matrix.CopyArray(gl_mat);
		glProgramUniformMatrix4fv(loc.first, loc.second, 1, GL_TRUE, gl_mat);
	}

	void set_uniform(const std::string& Name, const k3d::color& Color)
	{
		const loc_t loc = uniform_loc(Name);
		glProgramUniform3f(loc.first, loc.second, Color.red, Color.green, Color.blue);
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<program,
			k3d::interface_list<k3d::gl::iprogram> >factory(
			k3d::uuid("20d9a337-a2b3-4b53-8c1d-541ed9d71e99"),
			"OpenGLProgram",
			_("An OpenGL rendering pipeline object"),
			"OpenGL",
			k3d::iplugin_factory::EXPERIMENTAL);

		return factory;
	}

	void schedule_update()
	{
		m_need_use_stages = true;
	}


private:
	void ensure_created()
	{
		if(m_program_id == 0)
		{
			glGenProgramPipelines(1, &m_program_id);
		}
	}

	void use_program_stage(const GLbitfield ShaderBit, const GLuint shader_id)
	{
		if(!glIsProgram(shader_id))
			throw detail::invalid_shader_program("Shader program was invalid");
		glUseProgramStages(m_program_id, ShaderBit, shader_id);

		GLint nb_uniforms;
		glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &nb_uniforms);
		for(GLint i = 0; i != nb_uniforms; ++i)
		{
			GLchar name[512];
			GLsizei length;
			glGetActiveUniformName(shader_id, i, 512, &length, name);
			const GLint uniform_loc = glGetUniformLocation(shader_id, name);
			if(!m_uniform_map.insert(std::make_pair(name, std::make_pair(shader_id, uniform_loc))).second)
			{
				k3d::log() << error << "Uniform name " << name  << " already existed in program " << this->name() << std::endl;
			}
		}
	}

	std::pair<GLuint, GLint> uniform_loc(const std::string& Name)
	{
		uniform_map_t::const_iterator loc_it = m_uniform_map.find(Name);
		if(loc_it == m_uniform_map.end())
		{
			k3d::log() << warning << "Uniform name " << Name << " does not exist in program " << name() << std::endl;
			return std::make_pair(GLuint(0), GLint(-1));
		}
		return loc_it->second;
	}

	GLuint m_program_id;

	detail::shader_property m_vertex_shader;
	detail::shader_property m_fragment_shader;

	// Indicates if shaders have changed and the pipeline needs to be rebuilt
	k3d::bool_t m_need_use_stages;
	// Indicate if an error occurred
	k3d::bool_t m_error;

	// Store a mapping between uniform name and program and uniform ID
	typedef std::pair<GLuint, GLint> loc_t;
	typedef std::map< std::string, std::pair<GLuint, GLint> > uniform_map_t;
	uniform_map_t m_uniform_map;
};

detail::shader_property::shader_property(program* Owner, const char* Name, const char* Label, const char* Description, const GLenum Type, const GLbitfield PipelineBit) :
	m_shader(init_owner(*(Owner)) + init_name(Name) + init_label(Label) + init_description(Description) + init_value(static_cast<k3d::gl::ishader*>(nullptr))),
	m_program(Owner),
	m_type(Type),
	m_pipeline_bit(PipelineBit)
{
	m_shader.changed_signal().connect(boost::bind(&shader_property::on_shader_node_changed, this, _1));
	m_shader.connect_about_to_change_signal(boost::bind(&shader_property::on_shader_node_about_to_change, this, _1));
}

void detail::shader_property::on_shader_changed(k3d::ihint *)
{
	m_program->schedule_update();
}

k3d::iplugin_factory& program_factory()
{
	return program::get_factory();
}

} // namespace opengl

} // namespace k3d

#endif // !K3DSDK_GL_PROGRAM_H

