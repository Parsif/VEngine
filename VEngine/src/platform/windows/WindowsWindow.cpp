#include "precheader.h"

#include "WindowsWindow.h"



#include "core/Logger.h"
#include "events/KeyEvent.h"
#include "events/MouseEvents.h"
#include "events/WindowEvents.h"


namespace vengine
{
	WindowsWindow::~WindowsWindow()
	{
		glfwDestroyWindow(m_glfw_window);
		glfwTerminate();
	}

	void WindowsWindow::init(const std::string& title, unsigned int width, unsigned int height)
	{
		m_data.title = title;
		m_data.width = width;
		m_data.height = height;
		
		if (!glfwInit())
		{
			LOG_ERROR("Initialization failed");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
		m_glfw_window = glfwCreateWindow(m_data.width, m_data.height, title.c_str(), NULL, NULL);
		if (!m_glfw_window)
		{
			LOG_ERROR("Window or OpenGL context creation failed");
		}
		glfwMakeContextCurrent(m_glfw_window);

		glfwSetWindowUserPointer(m_glfw_window, &m_data);
		set_glfw_event_callbacks();
	
	}

	void WindowsWindow::set_glfw_event_callbacks() const
	{
		glfwSetWindowSizeCallback(m_glfw_window, [](GLFWwindow* window, int width, int height)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			const WindowResizedEvent event(width, height);
			data.width = width;
			data.height = height;
			data.event_callback(event);
		});

		glfwSetKeyCallback(m_glfw_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action)
			{
				case GLFW_PRESS:
				{
					const KeyPressedEvent event(key);
					data.event_callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					const KeyReleasedEvent event(key);
					data.event_callback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					//TODO: IMPLEMENT KEY REPEAT EVENT
					break;
				}
			}
		});

		glfwSetScrollCallback(m_glfw_window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			const MouseScrollEvent event(xoffset, yoffset);
			data.event_callback(event);
		});
		
		glfwSetCursorPosCallback(m_glfw_window, [](GLFWwindow* window, double x, double y)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			const MouseMoveEvent event(x - data.mouse_x,  data.mouse_y - y);

			data.mouse_x = x;
			data.mouse_y = y;

			data.event_callback(event);
		});

		glfwSetMouseButtonCallback(m_glfw_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			const MousePressedEvent event(button, action, mods);
			data.event_callback(event);
		});


		glfwSetDropCallback(m_glfw_window, [](GLFWwindow* window, int count, const char** paths)
		{
			auto& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			
			const FileDropEvent event(paths[0]);
			data.event_callback(event);
		});
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
		m_data.width = width;
		glfwSetWindowSize(m_glfw_window, m_data.width, m_data.height);
	}

	void WindowsWindow::set_height(unsigned int height)
	{
		m_data.height = height;
		glfwSetWindowSize(m_glfw_window, m_data.width, m_data.height);
	}

	void WindowsWindow::set_event_callback(const EventCallback& event_callback)
	{
		m_data.event_callback = event_callback;
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
