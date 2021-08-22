#include "precheader.h"
#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

namespace vengine
{
	//////////////////////////////////////////////////
	//Shader//////////////////////////////////////////
	//////////////////////////////////////////////////
	Shader::Shader(ShaderType type, const std::string& filename) : m_type(type)
	{
		std::ifstream shader_stream(filename);
		if (!shader_stream.is_open())
		{
			LOG_ERROR("Can't open file: " + filename);
			return;
		}
		const std::string shader_content((std::istreambuf_iterator<char>(shader_stream)), std::istreambuf_iterator<char>());
		shader_stream.close();

		m_render_id = glCreateShader(to_glew(type));
		const auto* const shader_source = shader_content.c_str();
		glShaderSource(m_render_id, 1, &shader_source, nullptr);
		glCompileShader(m_render_id);

		int success;
		glGetShaderiv(m_render_id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char info_log[512];
			glGetShaderInfoLog(m_render_id, sizeof(info_log), nullptr, info_log);
			LOG_ERROR(filename + " " + info_log);
		}

	}

	Shader::~Shader()
	{
		delete_shader();
	}

	void Shader::delete_shader()
	{
		if (m_render_id)
		{
			glDeleteShader(m_render_id);
			m_render_id = 0;
		}
	}

	GLenum Shader::to_glew(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::VERTEX: return GL_VERTEX_SHADER;
		case ShaderType::FRAGMENT: return GL_FRAGMENT_SHADER;
		case ShaderType::GEOMETRY: return GL_GEOMETRY_SHADER;
		case ShaderType::COMPUTE: return GL_COMPUTE_SHADER;
		}
	}
	
	//////////////////////////////////////////////////
	//ShaderProgram///////////////////////////////////
	//////////////////////////////////////////////////
	
	ShaderProgram::ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
	{
		Shader vertex_shader(ShaderType::VERTEX, vertex_shader_path);
		Shader fragment_shader(ShaderType::FRAGMENT, fragment_shader_path);

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
			LOG_ERROR(info_log);
        }
        
        vertex_shader.delete_shader();
        fragment_shader.delete_shader();
		set_all_uniform_locations();
	}

	ShaderProgram::ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path,
		const std::string& geometry_shader_path)
	{
		Shader vertex_shader(ShaderType::VERTEX, vertex_shader_path);
		Shader fragment_shader(ShaderType::FRAGMENT, fragment_shader_path);
		Shader geometry_shader(ShaderType::GEOMETRY, geometry_shader_path);

		m_render_id = glCreateProgram();
		glAttachShader(m_render_id, vertex_shader.get_render_id());
		glAttachShader(m_render_id, fragment_shader.get_render_id());
		glAttachShader(m_render_id, geometry_shader.get_render_id());
		glLinkProgram(m_render_id);

		int success;
		glGetProgramiv(m_render_id, GL_LINK_STATUS, &success);
		if (!success)
		{
			char info_log[512];
			glGetProgramInfoLog(m_render_id, sizeof(info_log), nullptr, info_log);
			LOG_ERROR(info_log);
		}

		vertex_shader.delete_shader();
		fragment_shader.delete_shader();
		geometry_shader.delete_shader();

		set_all_uniform_locations();
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
		case GL_SAMPLER_2D:
		case GL_SAMPLER_CUBE:
		{
			m_int_uniforms.emplace(uniform_name, UniformInfo<int>{location, type });
			break;
		}

		case GL_BOOL:
		{
			m_bool_uniforms.emplace(uniform_name, UniformInfo<bool>{location, type });
			break;
		}

		case GL_FLOAT:
		{
			m_float_uniforms.emplace(uniform_name, UniformInfo<float>{location, type });
			break;
		}

		case GL_FLOAT_VEC3:
		{
			m_vec3_uniforms.emplace(uniform_name, UniformInfo<glm::vec3>{location, type });
			break;
		}

		case GL_FLOAT_VEC4:
		{
			m_vec4_uniforms.emplace(uniform_name, UniformInfo<glm::vec4>{location, type });
			break;
		}

		case GL_FLOAT_MAT4:
		{
			m_mat4_uniforms.emplace(uniform_name, UniformInfo<glm::mat4>{location, type });
			break;
		}
			
		default:
		{
			LOG_ERROR("Unhandled uniform type");
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
		for (auto&& [name, uniform_info] : m_int_uniforms)
		{
			glUniform1i(uniform_info.location, uniform_info.value);
		}

		for (auto&& [name, uniform_info] : m_bool_uniforms)
		{
			glUniform1i(uniform_info.location, uniform_info.value);
		}
		
		for (auto&& [name, uniform_info] : m_float_uniforms)
		{
			glUniform1f(uniform_info.location, uniform_info.value);
		}
		
		for (auto&& [name, uniform_info] : m_vec3_uniforms)
		{
			glUniform3f(uniform_info.location, uniform_info.value.x, uniform_info.value.y,
						uniform_info.value.z);
		}
		
		for (auto&& [name, uniform_info] : m_vec4_uniforms)
		{
			glUniform4f(uniform_info.location, uniform_info.value.x, uniform_info.value.y,
						uniform_info.value.z, uniform_info.value.w);
		}
		
		for (auto&& [name, uniform_info] : m_mat4_uniforms)
		{
			glUniformMatrix4fv(uniform_info.location, 1, GL_FALSE, glm::value_ptr(uniform_info.value));
		}
		
	}

	//compute shader
	ComputeShader::ComputeShader(const std::string& shader_path)
	{
		Shader shader(ShaderType::COMPUTE, shader_path);

		m_render_id = glCreateProgram();
		glAttachShader(m_render_id, shader.get_render_id());
		glLinkProgram(m_render_id);

		int success;
		glGetProgramiv(m_render_id, GL_LINK_STATUS, &success);
		if (!success)
		{
			char info_log[512];
			glGetProgramInfoLog(m_render_id, sizeof(info_log), nullptr, info_log);
			LOG_ERROR(info_log);
		}

		shader.delete_shader();
	}

	void ComputeShader::use(uint32_t num_group_x, uint32_t num_group_y) const
	{
		glUseProgram(m_render_id); // Compute shader program.
		glDispatchCompute(num_group_x, num_group_y, 1);
	}

	void ComputeShader::set_image(const std::string& uniform_name, int image_id) const
	{
		auto location = glGetUniformLocation(m_render_id, uniform_name.c_str());
		glProgramUniform1i(m_render_id, location, image_id);
	}
}

