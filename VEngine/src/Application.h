#pragma once

#include "VEngine.h"



namespace VEngine

{
	class Application
	{
	public:
		Application();
		bool start();
		void run();

	private:
		Ref<Window> m_window;
		Ref<Renderer> m_renderer;
		
	};
}



