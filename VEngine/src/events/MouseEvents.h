#pragma once

#include "Event.h"

namespace vengine
{
	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(const double mouse_x, const double mouse_y) : m_mouse_x(mouse_x), m_mouse_y(mouse_y)
		{
			m_event_type = EventType::MOUSE_MOVED;
		}

		[[nodiscard]] auto get_mouse_x() const { return m_mouse_x; }
		[[nodiscard]] auto get_mouse_y() const { return m_mouse_y; }
	
	private:
		float m_mouse_x, m_mouse_y;
	};
	
	class MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(const double xoffset, const double yoffset) : m_xoffset(xoffset), m_yoffset(yoffset)
		{
			m_event_type = EventType::MOUSE_SCROLLED;
		}

		[[nodiscard]] auto get_xoffset() const { return m_xoffset; }
		[[nodiscard]] auto get_yoffset() const { return m_yoffset; }

	private:
		float m_xoffset, m_yoffset;
	};
	
}
