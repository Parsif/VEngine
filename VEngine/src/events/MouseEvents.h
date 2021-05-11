#pragma once

#include <utility>

#include "Event.h"

namespace vengine
{
	class MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(const double xoffset, const double yoffset) : m_xoffset(xoffset), m_yoffset(yoffset)
		{
			m_event_type = EventType::MOUSE_MOVED;
		}

		[[nodiscard]] auto get_xoffset() const { return m_xoffset; }
		[[nodiscard]] auto get_yoffset() const { return m_yoffset; }
	
	private:
		float m_xoffset, m_yoffset;
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


	class MousePressedEvent : public Event
	{
	public:
		MousePressedEvent(const int button, const int action, const int mods) :
		m_button(button), m_action(action), m_mods(mods)
		{
			m_event_type = EventType::MOUSE_PRESSED;
		}

		[[nodiscard]] auto get_button() const { return m_button; }
		[[nodiscard]] auto get_action() const { return m_action; }
		[[nodiscard]] auto get_mods() const { return m_mods; }

	private:
		int m_button, m_action, m_mods;
	};


	
}
