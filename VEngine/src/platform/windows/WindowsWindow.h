#pragma once

#include "precheader.h"

#include "core/Window.h"

#include "GLFW/glfw3.h"

namespace vengine
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const std::string& title = "VEngine", unsigned int width = 1280, unsigned int height = 720);
		virtual ~WindowsWindow();

		void on_update() override;
		
		[[nodiscard]] bool is_open() override;
		
		void set_width(unsigned int width) override;
		void set_height(unsigned int height) override;


		[[nodiscard]] unsigned int get_width() const override { return m_width; }
		[[nodiscard]] unsigned int get_height() const override { return m_height; }
		[[nodiscard]] void* get_native() const override;

		void swap_buffers() const override;

	private:
		GLFWwindow* m_glfw_window;
		unsigned int m_width, m_height;
	};

	inline Window* Window::create_window()
	{
		return new WindowsWindow();
	}

}



