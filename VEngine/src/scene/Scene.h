#pragma once

#include "renderer/Camera.h"

#include <entt/entt.hpp>

#include "Components.h"
#include "renderer/opengl/SkyboxGL.h"

namespace vengine
{
	class SceneHierarchyPanel;

	class Scene
	{
	public:
		void init();
		void on_update();
		void on_event(const Event& event);
		
		
		void set_camera(const Camera& camera);

		template<class T, class... Args>
		T& add_component(entt::entity entity, Args&&... args)
		{
			return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		void create_empty_entity();
		void create_camera();
		void create_cube();

		void draw_skybox();

		void destroy_entity(entt::entity entity);

	private:
		[[nodiscard]] auto& get_camera(){ return m_registry.get<CameraComponent>(m_camera_entity).camera; }

		
	private:
		entt::entity m_camera_entity;
		entt::registry m_registry;
		SkyboxGL m_skybox;
		
		friend SceneHierarchyPanel;
	};
}


