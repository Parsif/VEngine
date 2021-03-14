#include "precheader.h"
#include "SkyboxGL.h"

#include <stb_image.h>

#include "renderer/RenderCommand.h"

namespace vengine
{
	void SkyboxGL::init(const std::string& top_texture, const std::string& bottom_texture,
		const std::string& left_texture, const std::string& right_texture, const std::string& back_texture,
		const std::string& front_texture)
	{
		set_render_command();

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
		load_side_texture(top_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		load_side_texture(bottom_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		load_side_texture(right_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		load_side_texture(left_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		load_side_texture(front_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		load_side_texture(back_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	

	void SkyboxGL::bind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
	}

	void SkyboxGL::set_render_command()
	{
		float vertices[6 * 4] =
		{
			-1, -1, -1,
			1, -1, -1,
			1, 1, -1,
			-1, 1, -1,
			-1, -1, 1,
			1, -1, 1,
			1, 1, 1,
			-1, 1, 1
		};

		unsigned int indices[6 * 6] =
		{
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		m_render_command.set_vertex_buffer(vertices, sizeof(vertices));
		m_render_command.set_index_buffer(indices, sizeof(indices), sizeof(indices) / sizeof(unsigned int));
		m_render_command.set_buffer_layout(BufferLayout { ShaderDataType::Float3 });
	}


	void SkyboxGL::load_side_texture(const std::string& texture_path, GLenum texture_target)
	{
		int width, height, channels_count;
		unsigned char* data = stbi_load(texture_path.data(), &width, &height, &channels_count, 0);
		if (data)
		{
			glTexImage2D(texture_target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cerr << "Failed to load texture " << texture_path << '\n';
		}
		stbi_image_free(data);
	}
}
