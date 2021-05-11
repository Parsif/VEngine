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

		[[nodiscard]] static auto get_static_type() { return EventType::WINDOW_RESIZED; }

	
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


	class FileDropEvent : public Event
	{
	public:
		FileDropEvent(std::string path) : m_path(std::move(path))
		{
			m_event_type = EventType::FILE_DROP;
		}

		[[nodiscard]] auto& get_path() { return m_path; }

	private:
		std::string m_path;
	};
}
