#include "precheader.h"
#include "ShaderProgram.h"


namespace VEngine
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
	}

	void ShaderProgram::use() const
	{
        glUseProgram(m_render_id);
	}
}

