#pragma once
#include "RenderCommand.h"

namespace vengine
{
	//TODO: consider making grid and skybox static
	class Grid
	{
	public:
		void init();
		[[nodiscard]] auto& get_render_command() { return m_render_command; }

	public:
		inline static bool s_enabled = true;
		
	private:
		RenderCommand m_render_command;
		
	};
}

