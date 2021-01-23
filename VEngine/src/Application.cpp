#include "precheader.h"

#include "Application.h"



//TODO: remove include
#include "renderer/TriangleCommand.h"




namespace VEngine
{
	Application::Application()
	{
		m_window = std::unique_ptr<Window>(Window::create_window());
		m_renderer = std::make_unique<Renderer>();

	}

	bool Application::start()
	{
		return true;
	}

	void Application::run()
	{
		m_renderer->set_viewport(0, 0, m_window->get_width(), m_window->get_height());
		
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
		
		
		Shader vert{ShaderType::VERTEX, "./VEngine/src/renderer/shaders/basic.vert"};
		Shader frag{ShaderType::FRAGMENT, "./VEngine/src/renderer/shaders/basic.frag"};

		ShaderProgram sh{vert, frag};
		auto basic_material = std::make_shared<Material>(sh);

		auto triangle_command = std::make_shared<TriangleCommand>();
		triangle_command->set_vertex_buffer(vertices, sizeof(vertices));
		triangle_command->set_index_buffer(indices, sizeof(indices), sizeof(indices) / sizeof(*indices));
		triangle_command->set_material(basic_material);

		m_renderer->add_command(triangle_command);
		
		while(m_window->is_open())
		{
			m_window->on_update();
			m_renderer->render();
			m_window->swap_buffers();
		}
	}
}
