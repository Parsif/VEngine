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
		
		switch (m_type)
		{
		case aiTextureType_DIFFUSE:
			m_string_type = "albedo_map";
			break;

		case aiTextureType_METALNESS:
			m_string_type = "metallic_map";
			break;

		case aiTextureType_SHININESS:
			m_string_type = "roughness_map";
			break;

		case aiTextureType_AMBIENT_OCCLUSION:
			m_string_type = "ao_map";
			break;

		case aiTextureType_NORMALS:
			m_string_type = "normal_map";
			break;
		}
		
	}

	TextureGL::TextureGL(const unsigned int id) : m_id(id)
	{
	}

	void TextureGL::	bind(const GLenum slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

}
