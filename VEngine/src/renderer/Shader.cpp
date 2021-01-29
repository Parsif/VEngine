#include "precheader.h"

#include "Shader.h"

namespace vengine
{
	Shader::Shader(ShaderType type, const std::string& filename) : m_type(type)
	{
        std::ifstream shader_stream(filename);
        if (!shader_stream.is_open())
        {
            std::cerr << "Can't open file: " << filename << '\n';
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
            std::cerr << filename << " " << info_log << '\n';
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
		}
	}
}
