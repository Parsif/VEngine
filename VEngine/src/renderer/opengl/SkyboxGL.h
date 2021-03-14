#pragma once

#include <GL/glew.h>

#include "renderer/RenderCommand.h"

namespace vengine
{
	class SkyboxGL
	{
	public:
		void init(const std::string& top_texture, const std::string& bottom_texture, const std::string& left_texture,
			const std::string& right_texture, const std::string& back_texture, const std::string& front_texture);

		void bind() const;

		[[nodiscard]] auto& get_render_command() { return m_render_command; }
	
	private:
		void set_render_command();
		static void load_side_texture(const std::string& texture_path, GLenum texture_target);
	
	private:
		unsigned int m_id;
		RenderCommand m_render_command;

	};
	
}

