#include "precheader.h"
#include "Scene.h"

#include "renderer/TriangleCommand.h"
#include "renderer/Renderer.h"

namespace vengine
{
	
	void Scene::on_update()
	{
		create_command();
	}

	void Scene::on_event(const Event& event)
	{
		m_camera.on_event(event);
	}

	void Scene::create_command()
	{
		float vertices[] = {
			0.5f,  0.5f, 0.0f,  // Top Right
			0.5f, -0.5f, 0.0f,  // Bottom Right
		   -0.5f, -0.5f, 0.0f,  // Bottom Left
		   -0.5f,  0.5f, 0.0f   // Top Left 
		};

		unsigned int indices[] = {  // Note that we start from 0!
			0, 1, 3,  // First Triangle
			1, 2, 3   // Second Triangle	
		};

		glm::mat4 transform{1};

		Shader vert{ ShaderType::VERTEX, "./VEngine/src/renderer/shaders/basic.vert" };
		Shader frag{ ShaderType::FRAGMENT, "./VEngine/src/renderer/shaders/basic.frag" };

		ShaderProgram sh{ vert, frag };
		Material material{ sh };
		material.set("u_transform", transform);
		material.set("u_view_projection", m_camera.get_view_projection());

		auto triangle_command = std::make_shared<TriangleCommand>();
		triangle_command->set_vertex_buffer(vertices, sizeof(vertices));
		triangle_command->set_index_buffer(indices, sizeof(indices), sizeof(indices) / sizeof(*indices));
		triangle_command->set_material(material);

		Renderer::get_instance()->add_command(triangle_command);
	}

	void Scene::set_camera(const Camera& camera)
	{
		m_camera = camera;
	}
}
