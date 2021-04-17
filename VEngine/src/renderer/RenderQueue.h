#pragma once

#include "RenderCommand.h"


namespace vengine
{
	struct Drawable
	{
		std::vector<RenderCommand> commands;
		glm::mat4 transform;
		Material render_material;
		Material shadow_material;

	};
	
	class RenderQueue
	{
	public:
		void push_back(const Drawable& render_command);
		[[nodiscard]] size_t size() const { return m_drawables.size(); }
		void clear();

		[[nodiscard]] auto begin() const { return m_drawables.cbegin(); }
		[[nodiscard]] auto end() const { return m_drawables.cend(); }

		[[nodiscard]] auto begin() { return m_drawables.begin(); }
		[[nodiscard]] auto end() { return m_drawables.end(); }

	private:
	
	private:
		std::deque<Drawable> m_drawables;
		
	};
}


