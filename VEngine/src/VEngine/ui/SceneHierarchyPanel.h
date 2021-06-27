#pragma once

#include "core/Vtypes.h"
#include "core/Window.h"
#include "scene/Scene.h"

namespace vengine
{
	class SceneHierarchyPanel
	{
	public:
		void init(Ref<Window> window, Ref<Scene> scene);
		void draw();
		void draw_entity_node(entt::entity entity);
		void draw_entity_components(entt::entity entity) const;

		[[nodiscard]] auto get_selected_entity() const { return m_selected_entity; }

	private:
		static bool draw_vec3_component(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float column_width = 100.0f);
		void draw_material_texture_component(TextureGL& texture, aiTextureType texture_type, const std::string& label) const;
	
	private:
		Ref<Window> m_window;
		Ref<Scene> m_scene;
		entt::entity m_selected_entity{};
	};
}


