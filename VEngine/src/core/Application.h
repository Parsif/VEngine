#pragma once

#include "VEngine.h"



namespace vengine

{
	class Application
	{
	public:
		Application();
		bool start();
		void run();

	private:
		Ref<Window> m_window;
		Scope<Renderer> m_renderer;
		Scope<ImGuiUI> m_editor_ui;
		
	};
}



