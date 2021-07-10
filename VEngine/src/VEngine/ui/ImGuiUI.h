#pragma once

#include "ConsolePanel.h"
#include "SceneSettingsPanel.h"
#include "SceneHierarchyPanel.h"
#include "core/Vtypes.h"
#include "core/Window.h"


namespace vengine

{
	class ImGuiUI 
	{
	public:
		void init(Ref<Window> window, Ref<Scene> scene, Ref<Renderer> renderer);
		
		void draw();
		void on_event(const Event& event);

		[[nodiscard]] bool is_scene_view_focused() const { return m_scene_view_focused; }

		void shutdown() const;

	private:
		void set_edin_black_style() const;
		void set_maya_style() const;

		void init_imgui() const;

		void begin_frame();
		void end_frame() const;

		void render_docking_begin() const;
		void draw_guizmo() const;
	
	private:
		Ref<Window> m_window;
		Ref<Scene> m_scene;
		Ref<Renderer> m_renderer;
		
		SceneHierarchyPanel m_scene_hierarchy_panel;
		ConsolePanel m_console_panel;
		SceneSettingsPanel m_scene_settings_panel;

		int m_guizmo_type = -1;
		bool m_scene_view_focused;
	};
}

