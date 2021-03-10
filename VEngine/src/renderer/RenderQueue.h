#pragma once

#include "RenderCommand.h"
#include "core/Vtypes.h"


namespace vengine
{
	class RenderQueue
	{
	public:
		void push_back(const RenderCommand& render_command);
		[[nodiscard]] size_t size() const { return m_render_commands.size(); };
		void sort();
		void clear();

		[[nodiscard]] auto begin() const { return m_render_commands.cbegin(); }
		[[nodiscard]] auto end() const { return m_render_commands.cend(); }

		[[nodiscard]] auto begin() { return m_render_commands.begin(); }
		[[nodiscard]] auto end() { return m_render_commands.end(); }

	private:
	
	private:
		std::deque<RenderCommand> m_render_commands;
		
	};
}


