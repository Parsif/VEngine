#pragma once

#include "RenderCommand.h"


namespace vengine
{
	struct Mesh
	{
		std::vector<RenderCommand> commands;
		bool is_casting_shadow;
		glm::mat4 transform;
	};
	
	class RenderQueue
	{
	public:
		void push_back(const Mesh& render_command);
		[[nodiscard]] size_t size() const { return m_drawables.size(); }
		void clear();

		[[nodiscard]] auto begin() const { return m_drawables.cbegin(); }
		[[nodiscard]] auto end() const { return m_drawables.cend(); }

		[[nodiscard]] auto begin() { return m_drawables.begin(); }
		[[nodiscard]] auto end() { return m_drawables.end(); }

	private:
	
	private:
		std::deque<Mesh> m_drawables;
		
	};
}


