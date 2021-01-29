#include "precheader.h"

#include "WindowsWindow.h"


namespace vengine
{
	WindowsWindow::WindowsWindow(const std::string& title, unsigned int width, unsigned int height) : m_width(width), m_height(height)
	{
		if (!glfwInit())
		{
			std::cerr << " Initialization failed\n";
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		m_glfw_window = glfwCreateWindow(m_width, m_height, title.c_str(), NULL, NULL);
		if (!m_glfw_window)
		{
			std::cerr << "Window or OpenGL context creation failed\n";
		}
		glfwMakeContextCurrent(m_glfw_window);
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwDestroyWindow(m_glfw_window);
		glfwTerminate();
	}


	void WindowsWindow::on_update()
	{
		glfwPollEvents();
	}

	bool WindowsWindow::is_open()
	{
		return !glfwWindowShouldClose(m_glfw_window);
	}

	void WindowsWindow::set_width(unsigned int width)
	{
		m_width = width;
		glfwSetWindowSize(m_glfw_window, m_width, m_height);
	}

	void WindowsWindow::set_height(unsigned int height)
	{
		m_height = height;
		glfwSetWindowSize(m_glfw_window, m_width, m_height);
	}

	void WindowsWindow::swap_buffers() const
	{
		glfwSwapBuffers(m_glfw_window);
	}

	void* WindowsWindow::get_native() const
	{
		return m_glfw_window;
	}
}
