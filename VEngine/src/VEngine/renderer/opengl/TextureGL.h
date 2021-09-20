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
		void create(uint32_t width, uint32_t height);

		[[nodiscard]] std::string get_string_type() const;
		[[nodiscard]] auto get_id() const { return m_id; }
		[[nodiscard]] const std::string& get_filepath() const { return m_filepath; }

		explicit operator bool() const { return m_is_texture_loaded; }
		
	private:
		unsigned int m_id{};
		int m_width{ 0 }, m_height{ 0 }, m_channels{};

		std::string m_filepath;
		aiTextureType m_type = aiTextureType_NONE;
		bool m_is_texture_loaded = false;
	};
}


