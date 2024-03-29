#include "precheader.h"
#include "TextureGL.h"

#include "core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

namespace vengine
{
	TextureGL::TextureGL()
	{
		m_type = aiTextureType_NONE;
	}

	TextureGL::TextureGL(const std::string& filepath, aiTextureType type)
	{
		if(filepath.empty())
		{
			m_type = aiTextureType_NONE;
			return;
		}
		m_filepath = filepath;
		m_type = type;
		
		stbi_set_flip_vertically_on_load(true);
		void* data;
		GLenum internal_formant;
		GLenum data_type;
		if (stbi_is_hdr(filepath.c_str()))
		{
			data = stbi_loadf(filepath.data(), &m_width, &m_height, &m_channels, STBI_default);
			internal_formant = GL_RGB16F;
			data_type = GL_FLOAT;
		}
		else
		{
			data = stbi_load(filepath.data(), &m_width, &m_height, &m_channels, STBI_rgb);
			internal_formant = type == aiTextureType_DIFFUSE ? GL_SRGB : GL_RGB;
			data_type = GL_UNSIGNED_BYTE;
		}
		
		if(!data)
		{
			LOG_ERROR("Failed to load texture " + filepath);
			return;
		}
		
		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexImage2D(GL_TEXTURE_2D, 0, internal_formant, m_width, m_height, 0, GL_RGB, data_type, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		m_is_texture_loaded = true;
	}

	TextureGL::TextureGL(const unsigned int id) : m_id(id)
	{
	}

	void TextureGL::bind(const GLenum slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void TextureGL::create(uint32_t width, uint32_t height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTextureStorage2D(m_id, 1, GL_RGBA16F, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	std::string TextureGL::get_string_type() const
	{
		switch (m_type)
		{
		case aiTextureType_DIFFUSE:
			return "albedo_map";

		case aiTextureType_METALNESS:
			return "metallic_map";

		case aiTextureType_SHININESS:
			return "roughness_map";

		case aiTextureType_AMBIENT_OCCLUSION:
			return "ao_map";

		case aiTextureType_NORMALS:
			return "normal_map";


		}
	}
}
