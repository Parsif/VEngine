#pragma once

#include <assimp/material.h>
#include <GL/glew.h>

namespace vengine
{
	class TextureGL
	{
	public:
		enum class Texture2DType
		{
			DIFFUSE, SPECULAR, NORMAL
		};
	
	public:
		TextureGL() = default;
		TextureGL(const std::string& filepath, aiTextureType type);
		TextureGL(unsigned int id);


		void bind(GLenum slot = 0) const;

		[[nodiscard]] const std::string& get_string_type() const { return m_string_type; }

	private:
		unsigned int m_id;
		int m_width, m_height, m_channels;

		aiTextureType m_type;
		std::string m_string_type;
		
	};
}


