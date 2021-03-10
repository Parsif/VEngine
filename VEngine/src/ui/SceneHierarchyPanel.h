#pragma once

#include "core/Vtypes.h"
#include "scene/Scene.h"

namespace vengine
{
	class SceneHierarchyPanel
	{
	public:
		explicit SceneHierarchyPanel(Ref<Scene> scene);
		void draw();
		void draw_entity_node(entt::entity entity);
		void draw_entity_components(entt::entity entity) const;

	private:
		Ref<Scene> m_scene;
		entt::entity m_selected_entity{};
	};
}


