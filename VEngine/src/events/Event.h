#pragma once

namespace vengine
{
	class Event
	{
	public:
		enum class EventType
		{
			KEY_PRESSED, KEY_RELEASED,
			MOUSE_MOVED, MOUSE_SCROLLED,
			WINDOW_RESIZED, WINDOW_CLOSED
		};

	public:
		virtual ~Event() = default;
	
		[[nodiscard]] EventType get_type() const { return m_event_type; }
	
	protected:
		Event() = default;
		EventType m_event_type;
	};
}