#include "precheader.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace vengine
{
	ShaderProgram::ShaderProgram(Shader& vertex_shader, Shader& fragment_shader)
	{
        m_render_id = glCreateProgram();
        glAttachShader(m_render_id, vertex_shader.get_render_id());
        glAttachShader(m_render_id, fragment_shader.get_render_id());
        glLinkProgram(m_render_id);

        int success;
        glGetProgramiv(m_render_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetProgramInfoLog(m_render_id, sizeof(info_log), nullptr, info_log);
            std::cerr << info_log << '\n';
        }
        
        vertex_shader.delete_shader();
        fragment_shader.delete_shader();
		set_all_uniform_locations();
	}

	ShaderProgram::~ShaderProgram()
	{
		std::cout << "Deleting uniforms\n";

		for (auto&& uniform_info : m_uniforms_info)
		{
			//TODO: fix error
		//	delete uniform_info.second;
		}
	}

	void ShaderProgram::set_all_uniform_locations()
	{
		int num_of_uniforms;
		glGetProgramiv(m_render_id, GL_ACTIVE_UNIFORMS, &num_of_uniforms);
		
		constexpr unsigned int MAX_UNIFORM_NAME_LENGTH = 256;
		char* uniform_name = new char[MAX_UNIFORM_NAME_LENGTH];
		int length, count;
		unsigned int type;
		for (size_t i = 0; i < num_of_uniforms; ++i)
		{
			glGetActiveUniform(m_render_id, i, MAX_UNIFORM_NAME_LENGTH, &length, &count, &type, uniform_name);
			uniform_name[length] = '\0';
			set_uniform_location(uniform_name, type);
		}
		delete[] uniform_name;
	}

	void ShaderProgram::set_uniform_location(const std::string& uniform_name, unsigned int type)
	{
		const int location = glGetUniformLocation(m_render_id, uniform_name.c_str());
		switch (type)
		{
		case GL_INT:
		{
			m_uniforms_info[uniform_name] = new UniformInfo<int>(location, type);
			break;
		}

		case GL_FLOAT:
		{
			m_uniforms_info[uniform_name] = new UniformInfo<float>(location, type);
			break;
		}

		case GL_FLOAT_VEC3:
		{
			m_uniforms_info[uniform_name] = new UniformInfo<glm::vec3>(location, type);
			break;
		}

		case GL_FLOAT_VEC4:
		{
			m_uniforms_info[uniform_name] = new UniformInfo<glm::vec4>(location, type);
			break;
		}

		case GL_FLOAT_MAT4:
		{
			m_uniforms_info[uniform_name] = new UniformInfo<glm::mat4>(location, type);
			break;
		}

		default:
		{
			std::cerr << "Unhandled uniform type\n";
			break;
		}
		}
	}

	void ShaderProgram::use() const
	{
        glUseProgram(m_render_id);
	}

	void ShaderProgram::set_all_uniforms() const
	{
		std::cout << "Setting uniforms\n";
		for(auto&& uniform_info : m_uniforms_info)
		{
			switch (uniform_info.second->get_type())
			{
			case GL_INT:
			{
				auto* uniform = static_cast<UniformInfo<int>*>(uniform_info.second);
				glUniform1i(uniform->get_location(), uniform->get_value());
				break;
			}

			case GL_FLOAT:
			{
				auto* uniform = static_cast<UniformInfo<float>*>(uniform_info.second);
				glUniform1f(uniform->get_location(), uniform->get_value());
				break;
			}

			case GL_FLOAT_VEC3:
			{
				auto* uniform = static_cast<UniformInfo<glm::vec3>*>(uniform_info.second);
				auto value = uniform->get_value();
				glUniform3f(uniform->get_location(), value.x, value.y, value.z);
				break;
			}

			case GL_FLOAT_VEC4:
			{
				auto* uniform = static_cast<UniformInfo<glm::vec4>*>(uniform_info.second);
				auto value = uniform->get_value();
				glUniform4f(uniform->get_location(), value.x, value.y, value.z, value.w);
				break;
			}

			case GL_FLOAT_MAT4:
			{
				auto* uniform = static_cast<UniformInfo<glm::mat4>*>(uniform_info.second);
				glUniformMatrix4fv(uniform->get_location(), 1, GL_TRUE, glm::value_ptr(uniform->get_value()));
				break;
			}

			default:
			{
				std::cerr << "Unhandled uniform type\n";
				break;
			}
			}
		}
	}
}

