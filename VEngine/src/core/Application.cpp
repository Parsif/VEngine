#include "precheader.h"

#include "Application.h"


namespace vengine
{
	Application::Application()
	{
	}

	void Application::init()
	{
		m_window = std::shared_ptr<Window>(Window::create_window());
		m_window->init();
		m_window->set_event_callback([this](const Event& event) { on_event(event); });
		
		m_renderer.init();
		m_renderer.set_viewport(0, 0, m_window->get_width(), m_window->get_height());

		m_scene = std::make_shared<Scene>();
		m_scene->init();
		
		m_editor_ui.init(m_window, m_scene);
	}

	void Application::run()
	{
		
		while(m_window->is_open())
		{
			m_window->on_update();
			
			m_scene->on_update();
			m_renderer.render();
			m_editor_ui.draw();
			
			m_window->swap_buffers();
		}
	}

	void Application::shutdown()
	{
		m_renderer.shutdown();
	}

	void Application::on_event(const Event& event)
	{
		const EventDispatcher event_dispatcher(event);
		event_dispatcher.dispatch<EventType::WINDOW_RESIZED>([&]
		{
			on_window_resize(event);
		});

		event_dispatcher.dispatch<EventType::KEY_PRESSED>([&]
		{
			m_editor_ui.on_event(event);
		});

		event_dispatcher.dispatch<EventType::MOUSE_SCROLLED>([&]
		{
			m_scene->on_event(event);
		});
		
		event_dispatcher.dispatch<EventType::MOUSE_MOVED>([&]
		{
			if(glfwGetMouseButton(static_cast<GLFWwindow*>(m_window->get_native()), GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
			{
				m_scene->on_event(event);
			}
		});

		event_dispatcher.dispatch<EventType::MOUSE_PRESSED>([&]
		{
			m_scene->on_event(event);
		});
	}

	void Application::on_window_resize(const Event& event)
	{
		//TODO: fix framebuffer resizing
		const auto window_resize_event = *static_cast<const WindowResizedEvent*>(&event);
		m_renderer.set_viewport(0, 0, window_resize_event.get_width(), window_resize_event.get_height());
	}
}
