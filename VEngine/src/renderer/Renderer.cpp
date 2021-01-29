#include "precheader.h"

#include "Renderer.h"

#include "TriangleCommand.h"


namespace vengine
{
	Renderer* Renderer::s_instance = nullptr;
	Renderer::Renderer()
	{
		s_instance = this;
		m_render_pass_descriptor.need_clear_color = true;
	}

	void Renderer::add_command(const Ref<RenderCommand> render_command)
	{
		m_render_queue.push_back(render_command);
	}

	void Renderer::render()
	{
		m_renderer_api.begin_render_pass(m_render_pass_descriptor);
		m_current_frame_buffer = m_renderer_api.get_current_fbo();
		m_renderer_api.set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
		for(auto& render_command : m_render_queue)
		{
			process_render_command(render_command);
		}
		m_renderer_api.end_render_pass();
	}

	void Renderer::set_viewport(int x, int y, unsigned int width, unsigned int height)
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
	}

	void Renderer::process_render_command(const Ref<RenderCommand> command)
	{
		switch (command->get_type())
		{
		case RenderCommand::Type::MESH_COMMAND:
		{
			break;
		}
			
			
		case RenderCommand::Type::TRIANGLES_COMMAND:
		{
			const auto render_command = std::dynamic_pointer_cast<TriangleCommand>(command);
			m_renderer_api.set_vertex_buffer(render_command->get_vertex_buffer());
			m_renderer_api.set_index_buffer(render_command->get_index_buffer());
			m_renderer_api.set_vertex_array(render_command->get_vertex_array());
			m_renderer_api.set_material(render_command->get_material());
			

			m_renderer_api.draw_elements(render_command->get_primitive_type(), render_command->get_index_count(),
										   render_command->get_index_type(), render_command->get_index_offset());
			break;
		}
		
		default: ;
		}

	}
}
