#include "precheader.h"
#include "TextureGL.h"

#include "core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>

namespace vengine
{
	TextureGL::TextureGL(const std::string& filepath, aiTextureType type) : m_type(type)
	{
		unsigned char* data = stbi_load(filepath.data(), &m_width, &m_height, &m_channels, 0);
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		if (data)
		{
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
			m_string_type = "diffuse";
			break;

		case aiTextureType_SPECULAR:
			m_string_type = "specular";
			break;

		case aiTextureType_NORMALS:
			m_string_type = "normals";
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
