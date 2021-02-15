#pragma once


namespace vengine
{
	enum class EventType
	{
		KEY_PRESSED, KEY_RELEASED,
		MOUSE_MOVED, MOUSE_SCROLLED,
		WINDOW_RESIZED, WINDOW_CLOSED
	};
	
	class Event
	{
	public:
		virtual ~Event() = default;
	
		[[nodiscard]] EventType get_type() const { return m_event_type; }
		
	
	protected:
		Event() = default;
		EventType m_event_type;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(const Event& event) : m_event(event) {}


		// return true if event dispatched, false if not
		template<EventType T>
		[[nodiscard]] bool dispatch(const std::function<void()>& event_fn) const
		{
			if(T == m_event.get_type())
			{
				event_fn();
				return true;
			}
			return false;
		}
		
	private:
		Event m_event;
	};
}
