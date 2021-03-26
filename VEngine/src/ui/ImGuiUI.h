#pragma once

#include "SceneHierarchyPanel.h"
#include "core/Vtypes.h"
#include "core/Window.h"


namespace vengine

{
	class ImGuiUI 
	{
	public:
		void init(Ref<Window> window, Ref<Scene> scene);
		
		void draw();
		void on_event(const Event& event);
		
		~ImGuiUI();

	private:
		void init_imgui() const;

		void begin_frame();
		void end_frame() const;

		void render_docking_begin() const;
		void draw_guizmo() const;
	
	private:
		Ref<Window> m_window;
		Ref<Scene> m_scene;
		
		Scope<SceneHierarchyPanel> m_scene_hierarchy_panel;
		int m_guizmo_type = -1;
	};
}

