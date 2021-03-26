#pragma once
#include "events/Event.h"

namespace vengine
{
	class Window
	{
	public:
		using EventCallback = std::function<void(const Event&)>;

		virtual ~Window() = default;

		virtual void init(const std::string& title = "VEngine", unsigned int width = 1280, unsigned int height = 720) = 0;
		virtual void on_update() = 0;

		[[nodiscard]] virtual bool is_open() = 0;
		
		virtual void set_width(unsigned int width) = 0;
		virtual void set_height(unsigned int height) = 0;
		virtual void set_event_callback(const EventCallback& event_callback) = 0;

		[[nodiscard]] virtual unsigned int get_width() const = 0;
		[[nodiscard]] virtual unsigned int get_height() const = 0;
		[[nodiscard]] virtual bool is_key_pressed(int key_code) const = 0;

		virtual void swap_buffers() const = 0;

		[[nodiscard]] virtual void* get_native() const = 0;

		[[nodiscard]] static Window* create_window();
	};

	
}

