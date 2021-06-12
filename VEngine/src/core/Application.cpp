#include "precheader.h"

#include "Application.h"

#include "renderer/opengl/RendererApiGL.h"

namespace vengine
{
	void Application::init()
	{
		m_window = std::shared_ptr<Window>(Window::create_window());
		m_window->init();
		m_window->set_event_callback([this](const Event& event) { on_event(event); });

		RendererApiGL::init();
		
		m_renderer = std::make_shared<Renderer>();
		m_renderer->init();
		m_renderer->set_viewport(0, 0, m_window->get_width(), m_window->get_height());

		m_scene = std::make_shared<Scene>();
		m_scene->init(m_renderer);
		
		m_editor_ui.init(m_window, m_scene, m_renderer);
			
		Input::init(m_window);
	}

	void Application::run()
	{
		while(m_window->is_open())
		{
			{
				Timer timer{ "Frame time" };
				m_window->on_update();
				m_scene->on_update();
				m_renderer->render();
				m_editor_ui.draw();
				m_window->swap_buffers();
			}

		}
	}

	void Application::shutdown() const
	{
		m_renderer->shutdown();
		m_editor_ui.shutdown();
	}

	void Application::on_event(const Event& event)
	{
		switch (event.get_type())
		{
		case EventType::WINDOW_RESIZED:
		{
			on_window_resize(event);
			break;
		}

		case EventType::KEY_PRESSED:
		{
			m_editor_ui.on_event(event);
			break;
		}

		case EventType::MOUSE_SCROLLED:
		{
			//TODO: move focus check in scene
			if (m_editor_ui.is_scene_view_focused())
			{
				m_scene->on_event(event);
			}
			break;
		}

		case EventType::MOUSE_MOVED:
		{
			if (m_editor_ui.is_scene_view_focused())
			{
				m_scene->on_event(event);
			}
			break;
		}

		case EventType::MOUSE_PRESSED:
		{
			LOG_INFO("Click");
			if (m_editor_ui.is_scene_view_focused())
			{
				m_scene->on_event(event);
			}
			break;
		}

		case EventType::FILE_DROP:
		{
			on_drop_file(event);
			break;
		}
			
		}
	}

	void Application::on_window_resize(const Event& event) const
	{
		const auto window_resize_event = *static_cast<const WindowResizedEvent*>(&event);
		if(window_resize_event.get_width() != 0 && window_resize_event.get_height() != 0)
		{
			m_renderer->set_viewport(0, 0, window_resize_event.get_width(), window_resize_event.get_height());
			m_scene->on_event(event);
		}
	}

	void Application::on_drop_file(const Event& event) const
	{
		auto drop_file_event = *static_cast<const FileDropEvent*>(&event);
		SceneSerializer::deserialize(drop_file_event.get_path(), m_scene);
	}
}
