#include "precheader.h"

#include "Application.h"


namespace vengine
{
	Application::Application()
	{
		m_window = std::shared_ptr<Window>(Window::create_window());
	}

	void Application::start()
	{
		m_window->init();
		m_renderer.init();
		m_editor_ui.init(m_window);

		m_window->set_event_callback([this](const Event& event) { on_event(event); });
	}

	void Application::run()
	{
		m_renderer.set_viewport(0, 0, m_window->get_width(), m_window->get_height());

		const float aspect_ratio = float(m_window->get_width()) / m_window->get_height();
		auto projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 100.f);
		const Camera camera{ projection, glm::vec3(0.0f, 0.0f, 3.0f),
			glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) };

		m_scene.set_camera(camera);
	
		
		while(m_window->is_open())
		{
			m_window->on_update();
			
			m_scene.on_update();
			m_renderer.render();
			m_editor_ui.draw();
			
			m_window->swap_buffers();
		}
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
			m_scene.on_event(event);
		});
	}

	void Application::on_window_resize(const Event& event)
	{
		//TODO: fix framebuffer resizing
		const auto window_resize_event = *static_cast<const WindowResizedEvent*>(&event);
		m_renderer.set_viewport(0, 0, window_resize_event.get_width(), window_resize_event.get_height());
	}
}
