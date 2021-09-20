#include "precheader.h"
#include "Scene.h"

#include "renderer/ModelLoader.h"
#include "renderer/Renderer.h"

#include "renderer/MaterialLibrary.h"

#include <glm/gtx/string_cast.hpp>

namespace vengine
{
	void Scene::init(Ref<Renderer> renderer)
	{
		m_renderer = renderer;
		create_camera();
	}
	
	void Scene::on_update()
	{
		//Try to move setting params out from loop
		m_renderer->set_camera_params(m_active_camera);
		for (auto&& [view_entity, dir_light_component, transform_component] : m_registry.view<DirLightComponent, TransformComponent>().each())
		{
			m_renderer->add_dir_light(dir_light_component, transform_component.translation);
		}

		for (auto&& [view_entity, point_light_component, transform_component] : m_registry.view<PointLightComponent, TransformComponent>().each())
		{
			m_renderer->add_point_light(point_light_component, transform_component.translation);
		}

		for (auto&& [view_entity, sphere_area_light_component, transform_component] : m_registry.view<SphereAreaLightComponent, TransformComponent>().each())
		{
			m_renderer->add_sphere_area_light(sphere_area_light_component, transform_component.translation);
		}

		for (auto&& [view_entity, tube_area_light_component, transform_component] : m_registry.view<TubeAreaLightComponent, TransformComponent>().each())
		{
			m_renderer->add_tube_area_light(tube_area_light_component, transform_component.translation);
		}

		for (auto&& [view_entity, model_component, transform_component, materials_component] 
			: m_registry.view<ModelComponent, TransformComponent, MaterialsComponent>().each())
		{
			auto& mesh = ModelLoader::get_mesh(model_component.filepath);

			mesh.is_casting_shadow = true;
			mesh.transform = transform_component.get_transform();
			mesh.materials = materials_component;
			m_renderer->add_drawable(mesh);
		}
	}

	
	void Scene::on_event(const Event& event)
	{
		m_active_camera.on_event(event);
	}
	
	[[nodiscard]] entt::entity Scene::create_empty_entity(const std::string& tag)
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, tag);
		return entity;
	}

	void Scene::create_camera()
	{
		m_game_camera_entity = m_registry.create();
		m_registry.emplace<TagComponent>(m_game_camera_entity, "Camera");
		const Camera camera{ 45.0f, 0.1f, 500.f };
		m_registry.emplace<CameraComponent>(m_game_camera_entity, camera);
		m_registry.emplace<TransformComponent>(m_game_camera_entity);
	}

	void Scene::create_model(const std::string& model_path)
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Model entity");
		m_registry.emplace<TransformComponent>(entity);
		m_registry.emplace<ModelComponent>(entity, model_path);
		m_registry.emplace<MaterialsComponent>(entity);
	}

	void Scene::destroy_entity(entt::entity entity)
	{
		m_registry.destroy(entity);
	}

	void Scene::set_game_camera_entity(entt::entity entity)
	{
		m_game_camera_entity = entity;
	}

	void Scene::clear()
	{
		m_registry.clear();
	}

	void Scene::create_dir_light()
	{
		if (m_registry.view<DirLightComponent>().size() == Renderer::get_max_lights())
		{
			LOG_WARNING("Too many lights")
				return;
		}
		const entt::entity entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Directional light");
		m_registry.emplace<TransformComponent>(entity); 
		m_registry.emplace<DirLightComponent>(entity);
	}

	void Scene::create_point_light()
	{
		if (m_registry.view<PointLightComponent>().size() == Renderer::get_max_lights())
		{
			LOG_WARNING("Too many lights")
			return;
		}
		const entt::entity entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Point light");
		m_registry.emplace<TransformComponent>(entity); 
		m_registry.emplace<PointLightComponent>(entity);
	}

	void Scene::create_sphere_area_light()
	{
		if (m_registry.view<SphereAreaLightComponent>().size() == Renderer::get_max_lights())
		{
			LOG_WARNING("Too many lights")
			return;
		}
		const entt::entity entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Sphere area light");
		m_registry.emplace<TransformComponent>(entity);
		m_registry.emplace<SphereAreaLightComponent>(entity);
	}
	
	void Scene::create_tube_area_light()
	{
		if(m_registry.view<TubeAreaLightComponent>().size() == Renderer::get_max_lights())
		{
			LOG_WARNING("Too many lights")
			return;
		}
		const entt::entity entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Tube area light");
		m_registry.emplace<TransformComponent>(entity);
		m_registry.emplace<TubeAreaLightComponent>(entity);
	}
	

	void Scene::start_game()
	{
		if(m_registry.valid(m_game_camera_entity) && m_registry.has<CameraComponent>(m_game_camera_entity))
		{
			m_is_gamemode_on = true;
			m_active_camera = m_registry.get<CameraComponent>(m_game_camera_entity).camera;
			m_active_camera.set_position(m_registry.get<TransformComponent>(m_game_camera_entity).translation);
		}
		else
		{
			LOG_ERROR("There is no camera in the scene")
		}
	}

	void Scene::stop_game()
	{
		m_is_gamemode_on = false;
		m_active_camera = m_editor_camera;
	}

	void Scene::set_environment_texture(const TextureGL& texture) const
	{
		m_renderer->set_scene_environment_map(texture);
	}

	void Scene::set_exposure(float exposure) const
	{
		m_renderer->set_exposure(exposure);
	}
}
