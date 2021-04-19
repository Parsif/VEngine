#include "precheader.h"

#include "Renderer.h"
#include "MaterialLibrary.h"
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
		m_render_pass_descriptor.need_culling = true;
		
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
		//TODO: make framebuffer creation only once
		//render shadowmap
		const auto viewport = m_viewport;
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		set_viewport_size(SHADOW_WIDTH, SHADOW_HEIGHT);
		m_depth_frame_buffer.create(FrameBufferSpecifications{ SHADOW_WIDTH, SHADOW_HEIGHT, FrameBufferType::DEPTH_ONLY });

		begin_render_pass(m_depth_frame_buffer);

		for(auto& drawable : m_render_queue)
		{
			if(drawable.is_casting_shadow)
			{
				render_shadow(drawable);
			}
		}
		end_render_pass(m_depth_frame_buffer);

		
		//render scene
		set_viewport_size(viewport.width, viewport.height);
		m_final_frame_buffer.create(FrameBufferSpecifications{ viewport.width, viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL });

		begin_render_pass(m_final_frame_buffer);
		for (auto& drawable : m_render_queue)
		{
			render_drawable(drawable);
		}
		end_render_pass(m_final_frame_buffer);
		
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

	void Renderer::begin_render_pass(const FrameBufferGL& frame_buffer)
	{
		frame_buffer.bind();
		m_renderer_api.begin_render_pass(m_render_pass_descriptor);
	}

	void Renderer::end_render_pass(const FrameBufferGL& frame_buffer) const
	{
		m_renderer_api.end_render_pass();
		frame_buffer.unbind();
	}

	void Renderer::render_drawable(Drawable& drawable) const
	{
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
			if(drawable.is_casting_shadow)
			{
				TextureGL depth_texture{ m_depth_frame_buffer.get_depth_attachment() };
				depth_texture.bind(i);
				drawable.render_material.set("u_shadow_map", (int)i);
			}
			drawable.render_material.use();
			m_renderer_api.draw_elements(render_command);
		}
	}

	void Renderer::render_shadow(Drawable& drawable) const
	{
		drawable.shadow_material.use();

		for (auto&& render_command : drawable.commands)
		{
			m_renderer_api.draw_elements(render_command);
		}

	}
}
