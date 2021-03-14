#include "precheader.h"
#include "Scene.h"

#include "renderer/ModelLoader.h"
#include "renderer/Renderer.h"

#include "renderer/MaterialLibrary.h"

#include "glm/gtx/string_cast.hpp"

namespace vengine
{
	void Scene::init()
	{
		MaterialLibrary::init();
		m_skybox.init("./VEngine/assets/SkyCubemap/skybox_top.jpg", "./VEngine/assets/SkyCubemap/skybox_bottom.jpg",
			"./VEngine/assets/SkyCubemap/skybox_left.jpg", "./VEngine/assets/SkyCubemap/skybox_right.jpg",
			"./VEngine/assets/SkyCubemap/skybox_back.jpg", "./VEngine/assets/SkyCubemap/skybox_front.jpg");
		create_camera();
		create_cube();
	}

	void Scene::on_update()
	{
		draw_skybox();

		auto& basic_material = MaterialLibrary::get_material("Basic");
		m_registry.each([&](auto entity)
		{
			if(m_registry.has<ModelComponent>(entity)) 
			{
				ModelComponent& model_component = m_registry.get<ModelComponent>(entity);
				
				const glm::mat4 transform = m_registry.has<TransformComponent>(entity) ? m_registry.get<TransformComponent>(entity).get_transform() : glm::mat4{1.0f};
				for (auto&& command : ModelLoader::get_model_commands(model_component.filepath))
				{
					basic_material.set("u_transform", transform);
					basic_material.set("u_view_projection", get_camera().get_view_projection());
					command.set_material(basic_material);
					Renderer::get_instance()->add_command(command);
				}
			}
		});
		
	}

	void Scene::on_event(const Event& event)
	{
		get_camera().on_event(event);
	}
	
	void Scene::create_empty_entity()
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Empty entity");
	}

	void Scene::create_camera()
	{
		m_camera_entity = m_registry.create();
		m_registry.emplace<TagComponent>(m_camera_entity, "Camera");

		const auto viewport = Renderer::get_instance()->get_viewport();
		const float aspect_ratio = float(viewport.width) / viewport.height;
		const Camera camera{ 45.0f, aspect_ratio, 0.1f, 100.f,
			glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f) };
		
		m_registry.emplace<CameraComponent>(m_camera_entity, camera);
	}

	void Scene::create_cube()
	{
		const auto entity = m_registry.create();
		m_registry.emplace<TagComponent>(entity, "Cube entity");
		m_registry.emplace<TransformComponent>(entity);
		m_registry.emplace<ModelComponent>(entity, "./VEngine/assets/backpack/backpack.obj");
	}

	void Scene::draw_skybox()
	{
		auto& render_command = m_skybox.get_render_command();
		auto material = MaterialLibrary::get_material("Skybox");
		const glm::mat4 view = glm::mat3(get_camera().get_view());
		material.set("u_view_projection", get_camera().get_projection() * view);
		render_command.set_material(material);
		Renderer::get_instance()->add_command(render_command);
	}

	void Scene::destroy_entity(entt::entity entity)
	{
		m_registry.destroy(entity);
	}
}
