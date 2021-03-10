#pragma once

#include "core/Window.h"

#include "GLFW/glfw3.h"

namespace vengine
{
	class WindowsWindow : public Window
	{
	
	public:
		virtual ~WindowsWindow();

		void init(const std::string& title, unsigned int width, unsigned int height) override;
		void on_update() override;
		
		[[nodiscard]] bool is_open() override;
		
		void set_width(unsigned int width) override;
		void set_height(unsigned int height) override;

		void set_event_callback(const EventCallback& event_callback) override;


		[[nodiscard]] unsigned int get_width() const override { return m_data.width; }
		[[nodiscard]] unsigned int get_height() const override { return m_data.height; }
		[[nodiscard]] void* get_native() const override;

		void swap_buffers() const override;
	private:
		void set_glfw_event_callbacks() const;
		
	private:
		GLFWwindow* m_glfw_window;

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			EventCallback event_callback;
		};
		WindowData m_data;
	};

	

	inline Window* Window::create_window()
	{
		return new WindowsWindow();
	}

}



