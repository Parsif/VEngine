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
		m_grid.init();
		create_camera();
		m_renderer->set_skybox(SkyboxGL{ "./VEngine/assets/SkyCubemap/skybox_top.jpg", "./VEngine/assets/SkyCubemap/skybox_bottom.jpg",
			"./VEngine/assets/SkyCubemap/skybox_left.jpg", "./VEngine/assets/SkyCubemap/skybox_right.jpg",
			"./VEngine/assets/SkyCubemap/skybox_back.jpg", "./VEngine/assets/SkyCubemap/skybox_front.jpg" });
		m_registry.on_construct<DirLightComponent>().connect<&Scene::on_dir_light_update>(this);
		m_registry.on_update<DirLightComponent>().connect<&Scene::on_dir_light_update>(this);
	}
	
	void Scene::on_update()
	{
		if (Grid::s_enabled)
		{
			//TODO: fix grid
			//draw_grid();
		}

		m_registry.each([&](auto entity)
		{
			if(m_registry.has<ModelComponent>(entity)) 
			{
				ModelComponent& model_component = m_registry.get<ModelComponent>(entity);
				auto& mesh = ModelLoader::get_mesh(model_component.filepath);

				mesh.is_casting_shadow = true;
				mesh.transform = m_registry.get<TransformComponent>(entity).get_transform();
				mesh.materials = m_registry.get<MaterialsComponent>(entity);
				m_renderer->add_drawable(mesh);
			}
		});
	}

	
	void Scene::on_event(const Event& event)
	{
		Camera& camera = get_camera();
		camera.on_event(event);
		m_renderer->set_camera_params(camera.get_view_projection(), camera.get_position());
	}
	
	[[nodiscard]] entt::entity Scene::create_empty_entity(const std::string& tag)
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, tag);
		return entity;
	}

	void Scene::create_camera()
	{
		m_camera_entity = m_registry.create();
		m_registry.emplace<TagComponent>(m_camera_entity, "Camera");

		const Camera camera{ 45.0f, 0.1f, 500.f };
		
		m_registry.emplace<CameraComponent>(m_camera_entity, camera);
	}

	void Scene::create_model(const std::string& model_path)
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Model entity");
		m_registry.emplace<TransformComponent>(entity);
		m_registry.emplace<ModelComponent>(entity, model_path);
		m_registry.emplace<MaterialsComponent>(entity);
	}


	void Scene::draw_grid()
	{
		auto& render_command = m_grid.get_render_command();
		auto& grid_material = MaterialLibrary::get_material("Grid");
		grid_material.set("u_view", get_camera().get_view());
		grid_material.set("u_projection", get_camera().get_projection());
	}

	void Scene::destroy_entity(entt::entity entity)
	{
		m_registry.destroy(entity);
	}

	void Scene::set_camera_entity(entt::entity entity)
	{
		m_camera_entity = entity;
		Camera& camera = get_camera();
		m_renderer->set_camera_params(camera.get_view_projection(), camera.get_position());
	}

	void Scene::clear()
	{
		m_registry.clear();
	}

	void Scene::create_dir_light()
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Directional light");
		m_registry.emplace<DirLightComponent>(entity);
	}

	void Scene::on_dir_light_update(entt::registry& registry, entt::entity entity) const
	{
		auto& dir_light_component = registry.get<DirLightComponent>(entity);
		m_renderer->set_dir_light(dir_light_component);
	}
}
