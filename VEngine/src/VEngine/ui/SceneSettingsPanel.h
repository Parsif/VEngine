#pragma once
#include "core/Vtypes.h"
#include "core/Window.h"
#include "scene/Scene.h"

namespace vengine
{
	class SceneSettingsPanel
	{
	public:
		void init(Ref<Window> window, Ref<Scene> scene);
		void draw();
		
	
	private:
		Ref<Window> m_window;
		Ref<Scene> m_scene;
		TextureGL m_environment_map_texture;
	};
	
}

