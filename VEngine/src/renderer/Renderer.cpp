#include "precheader.h"

#include "Renderer.h"
#include "MaterialLibrary.h"
#include "RenderCommand.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


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


	void Renderer::add_drawable(const Drawable& drawable)
	{
		m_render_queue.push_back(drawable);
	}

	void Renderer::render()
	{
		//render shadowmap
		m_render_pass_descriptor.frame_buffer_type = FrameBufferType::DEPTH_ONLY;
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		const auto viewport = m_viewport;
		set_viewport_size(SHADOW_WIDTH, SHADOW_HEIGHT);
		begin_render_pass();
		for(auto& drawable : m_render_queue)
		{
			render_shadow(drawable);
		}
		m_depth_buffer_attachment = m_renderer_api.get_current_fbo().get_depth_attachment();
		m_renderer_api.end_render_pass();

		
		//render scene
		m_render_pass_descriptor.frame_buffer_type = FrameBufferType::COLOR_DEPTH_STENCIL;
		set_viewport_size(viewport.width, viewport.height);

		begin_render_pass();
		for (auto& drawable : m_render_queue)
		{
			render_drawable(drawable);
		}
		m_color_buffer_attachment = m_renderer_api.get_current_fbo().get_color_attachment();
		m_renderer_api.end_render_pass();


		m_render_queue.clear();
	}


	void Renderer::set_viewport(int x, int y, unsigned int width, unsigned int height)
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
		m_renderer_api.set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
	}

	void Renderer::set_viewport_size(unsigned int width, unsigned int height)
	{
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
	
	}

	void Renderer::render_drawable(Drawable& drawable)
	{
		drawable.render_material.set("u_transform", drawable.transform);
		for (auto&& render_command : drawable.commands)
		{
			//TODO: try to move textures to materials
			const auto& textures2d = render_command.get_textures2d();
			size_t i = 0;
			for (;i < textures2d.size(); ++i)
			{
				textures2d[i].bind();
				drawable.render_material.set("u_material." + textures2d[i].get_string_type(), (int)i);
			}

			TextureGL depth_texture{ m_depth_buffer_attachment };
			depth_texture.bind(i);
			drawable.render_material.set("u_shadow_map", (int)i);
			
			render_command.set_material(drawable.render_material); // TODO: set material per drawable not per command
			m_renderer_api.draw_elements(render_command);
		}
	}

	void Renderer::render_shadow(Drawable& drawable)
	{
		drawable.shadow_material.set("u_transform", drawable.transform);
		for (auto&& render_command : drawable.commands)
		{
			render_command.set_material(drawable.shadow_material); // TODO: set material per drawable not per command
			m_renderer_api.draw_elements(render_command);
		}

		auto depth_attachment = m_renderer_api.get_current_fbo().get_depth_attachment(); // getting shadowmap
	}
}
