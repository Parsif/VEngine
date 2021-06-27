#pragma once
#include "Event.h"

namespace vengine
{
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(const int keycode) : m_keycode(keycode)
		{
			m_event_type = EventType::KEY_PRESSED;
		}

		[[nodiscard]] auto get_keycode() const { return m_keycode; }
	private:
		int m_keycode;
	};


	class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(const int keycode) : m_keycode(keycode)
		{
			m_event_type = EventType::KEY_RELEASED;
		}

		[[nodiscard]] auto get_keycode() const { return m_keycode; }
	private:
		int m_keycode;
	};
}
