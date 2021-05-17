#include "precheader.h"
#include "Renderer.h"

#include "MaterialLibrary.h"
#include "RenderCommand.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace vengine
{
	void Renderer::init()
	{
		m_render_pass_descriptor.depth_test_enabled = true;
		m_render_pass_descriptor.need_clear_color = true;
		m_render_pass_descriptor.need_clear_depth = true;
		m_render_pass_descriptor.need_culling = true;
		
		
		m_dir_light_shadow_map.create(m_shadow_map_specs);
	}

	void Renderer::shutdown()
	{
		RendererApiGL::shutdown();
	}

	void Renderer::add_drawable(const Mesh& drawable)
	{
		m_render_queue.push_back(drawable);
	}

	void Renderer::render()
	{
		//render shadowmaps
		const auto viewport = m_viewport;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_shadow_map_specs.width, m_shadow_map_specs.height);
		

		begin_render_pass(m_dir_light_shadow_map);
		for (auto& drawable : m_render_queue)
		{
			if (drawable.is_casting_shadow)
			{
				m_direct_shadow_map_material.set("u_transform", drawable.transform);
				m_direct_shadow_map_material.use();
				render_shadow(drawable);
			}
		}
		end_render_pass(m_dir_light_shadow_map);
		
		
		//render scene
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, viewport.width, viewport.height);
		
		begin_render_pass(m_final_frame_buffer);
		
		render_skybox();
		
		for (auto& mesh : m_render_queue)
		{
			render_mesh(mesh);
		}

		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_final_frame_buffer.get_id(), m_intermediate_frame_buffer.get_id());
		end_render_pass(m_final_frame_buffer);
		
		m_render_queue.clear();
	}


	void Renderer::set_viewport(int x, int y, unsigned int width, unsigned int height)
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);

		//TODO: move framebuffer creation
		m_final_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL, 4 });
		m_intermediate_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL, 1 });
	}

	void Renderer::set_viewport_size(unsigned int width, unsigned int height)
	{
		m_viewport.width = width;
		m_viewport.height = height;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);

		m_final_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL, 4 });
		m_intermediate_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL, 1 });
	}

	void Renderer::set_dir_light(DirLightComponent dir_lights)
	{
		m_dir_light = dir_lights;
		m_render_material.set("u_dirlight.position", m_dir_light.position);
		m_render_material.set("u_dirlight.color", m_dir_light.color);
		m_render_material.set("u_dirlight.intensity", m_dir_light.intensity);

		const float near_plane = 1.0f, far_plane = 50.0f;
		const glm::mat4 light_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

		glm::mat4 light_view = glm::lookAt(m_dir_light.position,
		                                   glm::vec3(0.0f, 0.0f, 0.0f),
		                                   glm::vec3(0.0f, 1.0f, 0.0f));

		const glm::mat4 light_space_matrix = light_projection * light_view;
		m_direct_shadow_map_material.set("u_light_space_matrix", light_space_matrix);
		m_render_material.set("u_dirlight.light_space_matrix", light_space_matrix);
	}

	void Renderer::set_camera_params(const glm::mat4& view_projection, const glm::vec3& position)
	{
		m_camera_view_projection = view_projection;
		m_camera_pos = position;
		
		m_render_material.set("u_view_projection", m_camera_view_projection);
		m_render_material.set("u_view_pos", m_camera_pos);
	}

	void Renderer::set_skybox(const SkyboxGL& skybox)
	{
		m_skybox = skybox;
	}

	void Renderer::begin_render_pass(const FrameBufferGL& frame_buffer)
	{
		frame_buffer.bind();
		RendererApiGL::begin_render_pass(m_render_pass_descriptor);
	}

	void Renderer::end_render_pass(const FrameBufferGL& frame_buffer) const
	{
		RendererApiGL::end_render_pass();
		frame_buffer.unbind();
	}

	//TODO: try to move textures to materials
	void Renderer::render_mesh(Mesh& mesh) const
	{
		m_render_material.set("u_transform", mesh.transform);

		for (auto&& render_command : mesh.commands)
		{
			size_t i = 0;
	
			//setting other textures
			for (const auto& texture_2d : render_command.get_textures2d())
			{
				m_render_material.set("u_material." + texture_2d.get_string_type(), (int)i);
				texture_2d.bind(i++);
			}

			m_render_material.set("u_dir_light_shadow_map", (int)i);
			TextureGL shadow_map_texture{ m_dir_light_shadow_map.get_depth_attachment() };
			shadow_map_texture.bind(i++);
			
			m_render_material.use();
			RendererApiGL::draw_elements(render_command);
		}
	}

	void Renderer::render_shadow(Mesh& mesh) const
	{
		for (auto&& render_command : mesh.commands)
		{
			RendererApiGL::draw_elements(render_command);
		}
	}

	void Renderer::render_skybox()
	{
		m_skybox_material.use();
		m_skybox.bind();
		RendererApiGL::draw_elements(m_skybox.get_render_command());
	}
}
