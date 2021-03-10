#include "precheader.h"
#include "Texture2dGL.h"

#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"

namespace vengine
{
	Texture2dGL::Texture2dGL(std::string_view filepath)
	{
		//stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filepath.data(), &m_width, &m_height, &m_channels, 0);
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cerr << "Failed to load texture " << filepath << '\n';
		}
		stbi_image_free(data);
	}

	void Texture2dGL::bind(const GLenum slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}
}
