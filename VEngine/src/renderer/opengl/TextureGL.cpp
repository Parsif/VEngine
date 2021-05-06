#include "precheader.h"
#include "TextureGL.h"

#include "core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

namespace vengine
{
	TextureGL::TextureGL(const std::string& filepath, aiTextureType type) : m_type(type)
	{
		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(filepath.data(), &m_width, &m_height, &m_channels, STBI_rgb);
		if (data)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
			glBindTexture(GL_TEXTURE_2D, m_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		
		else
		{
			Logger::log("Failed to load texture " + filepath, Logger::MessageSeverity::ERROR);
		}
		stbi_image_free(data);

		
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

	void TextureGL::bind(const GLenum slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

}
