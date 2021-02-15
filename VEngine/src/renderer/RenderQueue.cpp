#include "precheader.h"

#include "RenderQueue.h"


namespace vengine
{
	void RenderQueue::push_back(const Ref<RenderCommand> render_command)
	{
		m_render_commands.push_back(render_command);
	}

	void RenderQueue::clear()
	{
		m_render_commands.clear();
	}
}
