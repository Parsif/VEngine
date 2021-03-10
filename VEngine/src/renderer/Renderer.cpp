#include "precheader.h"

#include "Renderer.h"

#include "RenderCommand.h"


namespace vengine
{
	Renderer* Renderer::s_instance = nullptr;

	void Renderer::init()
	{
		s_instance = this;
		m_render_pass_descriptor.depth_test_enabled = true;
		m_render_pass_descriptor.need_clear_color = true;
		m_render_pass_descriptor.need_clear_depth = true;
		
		m_renderer_api.init();
	}

	void Renderer::shutdown()
	{
		m_renderer_api.shutdown();
	}

	void Renderer::add_command(const RenderCommand& render_command)
	{
		m_render_queue.push_back(render_command);
	}

	void Renderer::render()
	{
		begin_render_pass();
		for(auto& render_command : m_render_queue)
		{
			process_render_command(render_command);
		}
		end_render_pass();
	}

	void Renderer::set_viewport(int x, int y, unsigned int width, unsigned int height)
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
		m_renderer_api.set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
	}

	void Renderer::begin_render_pass()
	{
		m_renderer_api.begin_render_pass(m_render_pass_descriptor);
	}

	void Renderer::end_render_pass()
	{
		m_current_frame_buffer = m_renderer_api.get_current_fbo();
		m_renderer_api.end_render_pass();
		m_render_queue.clear();
	}

	void Renderer::process_render_command(RenderCommand& command) const
	{
		m_renderer_api.draw_elements(command);
	}
}
