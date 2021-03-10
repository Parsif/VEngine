#pragma once


#include <string_view>

#include <GL/glew.h>

namespace vengine
{
	class Texture2dGL
	{
	public:
		Texture2dGL() = default;
		Texture2dGL(std::string_view filepath);

		void bind(GLenum slot = 0) const;
		
	private:
		unsigned int m_id;
		int m_width, m_height, m_channels;
		
	};
}


