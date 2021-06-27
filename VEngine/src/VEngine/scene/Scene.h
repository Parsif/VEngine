#pragma once

#include "renderer/Camera.h"

#include "Components.h"
#include "renderer/Grid.h"
#include "renderer/Renderer.h"

#include <entt/entt.hpp>

namespace vengine
{
	class SceneHierarchyPanel;
	class ImGuiUI;
	class SceneSerializer;

	class Scene
	{
	public:
		void init(Ref<Renderer> renderer);
		void on_update();
		void on_event(const Event& event);

		template<class T, class... Args>
		T& add_component(entt::entity entity, Args&&... args)
		{
			return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		[[nodiscard]] entt::entity create_empty_entity(const std::string& tag = "Empty entity");
		void create_camera();
		void create_model(const std::string& model_path);

		void draw_grid();

		void destroy_entity(entt::entity entity);

		//TODO: consider other camera implementation inside scene && check component existence before getting it
		[[nodiscard]] auto& get_active_camera() { return m_active_camera; }

		void set_game_camera_entity(entt::entity entity);
		void clear();

		void create_dir_light();

		void start_game();
		void stop_game();
		[[nodiscard]] bool is_gamemode_on() const { return m_is_gamemode_on; };
	
	private:
		void on_dir_light_create_update(entt::registry& registry, entt::entity entity) const;
		void on_dir_light_destroy(entt::registry& registry, entt::entity entity) const;


	private:
		Camera m_editor_camera{45.0f, 0.1f, 500.f};
		Camera m_active_camera = m_editor_camera;
		entt::entity m_game_camera_entity;

		entt::registry m_registry;
		Grid m_grid;
		Ref<Renderer> m_renderer;

		bool m_is_gamemode_on = false;
		
		friend SceneHierarchyPanel;
		friend ImGuiUI;
		friend SceneSerializer;

	};
}


