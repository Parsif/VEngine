#pragma once
#include "RenderCommand.h"

namespace vengine
{
	class Grid
	{
	public:
		void init();
		[[nodiscard]] auto& get_render_command() { return m_render_command; }
		
	private:
		RenderCommand m_render_command;
	};
}

