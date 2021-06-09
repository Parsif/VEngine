#pragma once

#include <assimp/material.h>
#include <GL/glew.h>

namespace vengine
{
	class TextureGL
	{
	public:
		TextureGL();
		TextureGL(const std::string& filepath, aiTextureType type);
		TextureGL(unsigned int id);

		void bind(GLenum slot = 0) const;

		[[nodiscard]] const std::string& get_string_type() const { return m_string_type; }
		[[nodiscard]] auto get_id() const { return m_id; }
		[[nodiscard]] const std::string& get_filepath() const { return m_filepath; }

		explicit operator bool() const { return m_type != aiTextureType_NONE; }
		
	private:
		unsigned int m_id;
		int m_width{ 0 }, m_height{ 0 }, m_channels;

		std::string m_filepath;
		std::string m_string_type;
		aiTextureType m_type;
	};
}


