#pragma once

#include "VEngine.h"


namespace vengine

{
	class Application
	{
	public:
		Application();
		void start();
		void run();

		void on_event(const Event& event);

		void on_window_resize(const Event& event);

	private:
		Ref<Window> m_window;
		Renderer m_renderer;
		ImGuiUI m_editor_ui;
		Scene m_scene;
		
	};
}



