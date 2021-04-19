#include "precheader.h"
#include "Scene.h"

#include "renderer/ModelLoader.h"
#include "renderer/Renderer.h"

#include "renderer/MaterialLibrary.h"

#include <glm/gtx/string_cast.hpp>

namespace vengine
{
	void Scene::init()
	{
		m_renderer = Renderer::get_instance();

		MaterialLibrary::init();
		m_skybox.init("./VEngine/assets/SkyCubemap/skybox_top.jpg", "./VEngine/assets/SkyCubemap/skybox_bottom.jpg",
			"./VEngine/assets/SkyCubemap/skybox_left.jpg", "./VEngine/assets/SkyCubemap/skybox_right.jpg",
			"./VEngine/assets/SkyCubemap/skybox_back.jpg", "./VEngine/assets/SkyCubemap/skybox_front.jpg");
		m_grid.init();
		create_camera();
	}

	void Scene::on_update()
	{
		if (Grid::s_enabled)
		{
			//TODO: fix grid
			//draw_grid();
		}
		
		draw_skybox();

		auto& basic_material = MaterialLibrary::get_material("Basic");
		basic_material.set("u_view_projection", get_camera().get_view_projection());
		basic_material.set("u_view_pos", get_camera().get_position());

		glm::mat4 light_view;
		
		auto dir_light_view = m_registry.view<DirLightComponent>();

		for (auto dir_light_entity : dir_light_view)
		{
			auto& dir_light_component = dir_light_view.get<DirLightComponent>(dir_light_entity);
			basic_material.set("u_dirlight.position", dir_light_component.position);
			basic_material.set("u_dirlight.color", dir_light_component.color);
			light_view = glm::lookAt(dir_light_component.position,
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f));
		}
		
		const float near_plane = 1.0f, far_plane = 50.0f;
		const glm::mat4 light_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
		const glm::mat4 light_space_matrix = light_projection * light_view;

		auto& shadowmap_material = MaterialLibrary::get_material("Shadowmap");
		shadowmap_material.set("u_light_space_matrix", light_space_matrix);
		basic_material.set("u_light_space_matrix", light_space_matrix);


		m_registry.each([&](auto entity)
		{
			if(m_registry.has<ModelComponent>(entity)) 
			{
				ModelComponent& model_component = m_registry.get<ModelComponent>(entity);
				const auto& transform = m_registry.get<TransformComponent>(entity).get_transform();
				basic_material.set("u_transform", transform);
				shadowmap_material.set("u_transform", transform);

				m_renderer->add_drawable(Drawable{
					ModelLoader::get_model_commands(model_component.filepath),
					basic_material,
					true,
					shadowmap_material
				});
			}
		});
	}

	void Scene::on_event(const Event& event)
	{
		get_camera().on_event(event);
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
	}

	void Scene::draw_skybox()
	{
		auto& render_command = m_skybox.get_render_command();
		auto& skybox_material = MaterialLibrary::get_material("Skybox");
		m_renderer->add_drawable(Drawable{
			std::vector<RenderCommand>{render_command},
			skybox_material,
			false
		});
	}

	void Scene::draw_grid()
	{
		auto& render_command = m_grid.get_render_command();
		auto& grid_material = MaterialLibrary::get_material("Grid");
		grid_material.set("u_view", get_camera().get_view());
		grid_material.set("u_projection", get_camera().get_projection());
		m_renderer->add_drawable(Drawable{
			std::vector<RenderCommand>{render_command},
			grid_material,
			false
			});
	}

	void Scene::destroy_entity(entt::entity entity)
	{
		m_registry.destroy(entity);
	}

	void Scene::set_camera_entity(entt::entity entity)
	{
		m_camera_entity = entity;
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
}
