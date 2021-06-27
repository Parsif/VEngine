#pragma once

#include "VEngine.h"

namespace vengine
{
	class Application
	{
	public:
		void init();
		void run();
		void shutdown() const;

		void on_event(const Event& event);

		void on_window_resize(const Event& event) const;
		void on_drop_file(const Event& event) const;

	private:
		Ref<Window> m_window;
		Ref<Renderer> m_renderer;
		ImGuiUI m_editor_ui;
		Ref<Scene> m_scene;
		
	};
}



