#pragma once
#include "Event.h"

namespace vengine
{
	class WindowResizedEvent : public Event
	{
	public:
		WindowResizedEvent(unsigned int width, unsigned int height) : m_width(width), m_height(height)
		{
			m_event_type = EventType::WINDOW_RESIZED;
		}

		[[nodiscard]] auto get_width() const { return m_width; }
		[[nodiscard]] auto get_height() const { return m_height; }
	
	private:
		unsigned int m_width, m_height;
	};

	class WindowClosedEvent : public Event
	{
	public:
		WindowClosedEvent()
		{
			m_event_type = EventType::WINDOW_CLOSED;
		}
	};
}
