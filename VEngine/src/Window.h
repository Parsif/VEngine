#pragma once

#include <string>
#include <utility>

namespace VEngine
{
	class Window
	{
	public:
	
		virtual ~Window() = default;

		virtual void on_update() = 0;

		[[nodiscard]] virtual bool is_open() = 0;
		
		virtual void set_width(unsigned int width) = 0;
		virtual void set_height(unsigned int height) = 0;

		[[nodiscard]] virtual unsigned int get_width() const = 0;
		[[nodiscard]] virtual unsigned int get_height() const = 0;
		virtual void swap_buffers() const = 0;


		[[nodiscard]] static Window* create_window();
	protected:
	};

	
}

