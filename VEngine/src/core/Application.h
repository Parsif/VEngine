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
		std::unique_ptr<Renderer> m_renderer;
		std::unique_ptr<ImGuiUI> m_editor_ui;
		
	};
}



