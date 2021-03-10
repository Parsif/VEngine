#pragma once

#include "VEngine.h"


namespace vengine

{
	class Application
	{
	public:
		Application();
		void init();
		void run();
		void shutdown();

		void on_event(const Event& event);

		void on_window_resize(const Event& event);

	private:
		Ref<Window> m_window;
		Renderer m_renderer;
		ImGuiUI m_editor_ui;
		Ref<Scene> m_scene;
		
	};
}



