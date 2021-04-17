#include "precheader.h"

#include "RenderQueue.h"


namespace vengine
{
	void RenderQueue::push_back(const Drawable& drawable)
	{
		m_drawables.push_back(drawable);
	}

	void RenderQueue::clear()
	{
		m_drawables.clear();
	}
}
