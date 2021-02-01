#pragma once

#include "SceneHierarchyPanel.h"
#include "core/Vtypes.h"
#include "core/Window.h"


namespace vengine

{
	class ImGuiUI 
	{
	public:
		ImGuiUI(Ref<Window> window);
		void draw();
		~ImGuiUI();

	private:
		void init_imgui() const;

		void begin_frame();
		void end_frame();

		void render_docking_begin() const;
	
	private:
		Ref<Window> m_window;
		Scope<SceneHierarchyPanel> m_scene_hierarchy_panel;
	};
}

