#include "precheader.h"
#include "Renderer.h"

#include "RenderCommand.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "MaterialLibrary.h"
#include "opengl/RendererApiGL.h"

namespace vengine
{
	void Renderer::init()
	{
		m_render_pass_descriptor.depth_test_enabled = true;
		m_render_pass_descriptor.need_clear_color = true;
		m_render_pass_descriptor.need_clear_depth = true;
		m_render_pass_descriptor.need_culling = true;

		for (auto& shadow_map : m_dir_light_shadow_map_textures)
		{
			shadow_map.create(m_shadow_map_specs);
		}

		
		m_pbr_render_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/pbr.vert", 
													"./VEngine/src/VEngine/renderer/shaders/pbr.frag", "PBR");

		m_basic_render_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/basic.vert", 
			"./VEngine/src/VEngine/renderer/shaders/basic.frag", "Basic");


		m_skybox_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/skybox.vert",
			"./VEngine/src/VEngine/renderer/shaders/skybox.frag", "Skybox");

		m_direct_shadow_map_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/shadowmap.vert",
			"./VEngine/src/VEngine/renderer/shaders/shadowmap.frag", "Shadowmap");
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

		//for every dir light render shadow map
		for(size_t i = 0; i < m_dir_light_space_matrices.size(); ++i)
		{
			begin_render_pass(m_dir_light_shadow_map_textures[i]);
			for (auto& drawable : m_render_queue)
			{
				if (drawable.is_casting_shadow)
				{
					m_direct_shadow_map_material.set("u_transform", drawable.transform);
					m_direct_shadow_map_material.set("u_light_space_matrix", m_dir_light_space_matrices[i]);
					m_direct_shadow_map_material.use();
					render_shadow(drawable);
				}
			}
			end_render_pass(m_dir_light_shadow_map_textures[i]);
		}
		
		
		
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

	void Renderer::add_dir_light(const DirLightComponent& dir_lights, const glm::vec3& position)
	{
		if(m_dir_light_space_matrices.size() == MAX_NUMBER_OF_DIR_LIGHTS)
		{
			return;
		}
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_light_space_matrices.size()) + "].position", position);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_light_space_matrices.size()) + "].color", dir_lights.color);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_light_space_matrices.size()) + "].intensity", dir_lights.intensity);

		const float near_plane = 1.0f, far_plane = 50.0f;
		const glm::mat4 light_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

		glm::mat4 light_view = glm::lookAt(position,
		                                   glm::vec3(0.0f, 0.0f, 0.0f),
		                                   glm::vec3(0.0f, 1.0f, 0.0f));

		const glm::mat4 light_space_matrix = light_projection * light_view;
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_light_space_matrices.size()) + "].light_space_matrix", light_space_matrix);


		m_dir_light_space_matrices.push_back(light_space_matrix);
		m_pbr_render_material.set("u_number_of_dir_lights", (int)m_dir_light_space_matrices.size());
	}

	void Renderer::set_camera_params(const glm::mat4& view_projection, const glm::vec3& position)
	{
		m_camera_view_projection = view_projection;
		m_camera_pos = position;
		
		m_pbr_render_material.set("u_view_projection", m_camera_view_projection);
		m_pbr_render_material.set("u_view_pos", m_camera_pos);
	}

	void Renderer::set_skybox(const SkyboxGL& skybox)
	{
		m_skybox = skybox;
	}

	void Renderer::destroy_dir_lights()
	{
		for (size_t i = 0; i < m_dir_light_space_matrices.size(); ++i)
		{
			m_pbr_render_material.set("u_dirlights[" + std::to_string(i) + "].position", glm::vec3(0.0f));
			m_pbr_render_material.set("u_dirlights[" + std::to_string(i) + "].color", glm::vec3(0.0f));
			m_pbr_render_material.set("u_dirlights[" + std::to_string(i) + "].intensity", 0.0f);
			m_pbr_render_material.set("u_dirlights[" + std::to_string(i) + "].light_space_matrix", glm::mat4(0.0f));
		}
		m_pbr_render_material.set("u_number_of_dir_lights", (int)0);
		
		m_direct_shadow_map_material.set("u_light_space_matrix", glm::mat4(0.0f));

		m_dir_light_space_matrices.clear();
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
	void Renderer::render_mesh(Mesh& mesh) 
	{
		m_pbr_render_material.set("u_transform", mesh.transform);
		m_pbr_render_material.set("u_material.has_albedo", mesh.has_albedo_texture || mesh.materials.albedo_texture);
		m_pbr_render_material.set("u_material.has_metallic", mesh.has_metallic_texture || mesh.materials.metallic_texture);
		m_pbr_render_material.set("u_material.has_roughness", mesh.has_roughness_texture || mesh.materials.roughness_texture);
		m_pbr_render_material.set("u_material.has_ao", mesh.has_ao_texture || mesh.materials.ao_texture);
		m_pbr_render_material.set("u_material.has_normal_map", mesh.has_normal_texture || mesh.materials.normal_texture);
		
		for (auto&& render_command : mesh.commands)
		{
			size_t texture_slot = 0;
			if (mesh.materials.albedo_texture)
			{
				m_pbr_render_material.set("u_material." + mesh.materials.albedo_texture.get_string_type(), (int)texture_slot);
				mesh.materials.albedo_texture.bind(texture_slot++);
			}
			if (mesh.materials.metallic_texture)
			{
				m_pbr_render_material.set("u_material." + mesh.materials.metallic_texture.get_string_type(), (int)texture_slot);
				mesh.materials.metallic_texture.bind(texture_slot++);
			}
			if (mesh.materials.roughness_texture)
			{
				m_pbr_render_material.set("u_material." + mesh.materials.roughness_texture.get_string_type(), (int)texture_slot);
				mesh.materials.roughness_texture.bind(texture_slot++);
			}
			if (mesh.materials.ao_texture)
			{
				m_pbr_render_material.set("u_material." + mesh.materials.ao_texture.get_string_type(), (int)texture_slot);
				mesh.materials.ao_texture.bind(texture_slot++);
			}
			if (mesh.materials.normal_texture)
			{
				m_pbr_render_material.set("u_material." + mesh.materials.normal_texture.get_string_type(), (int)texture_slot);
				mesh.materials.normal_texture.bind(texture_slot++);
			}
			
			for (const auto& texture_2d : render_command.get_textures2d())
			{
				m_pbr_render_material.set("u_material." + texture_2d.get_string_type(), (int)texture_slot);
				texture_2d.bind(texture_slot++);
			}

			for (size_t i = 0; i < m_dir_light_space_matrices.size(); ++i)
			{
				m_pbr_render_material.set("u_dirlights[" + std::to_string(i) + "].shadow_map", (int)texture_slot);
				TextureGL shadow_map_texture{ m_dir_light_shadow_map_textures[i].get_depth_attachment() };
				shadow_map_texture.bind(texture_slot++);
			}
			
			
			m_pbr_render_material.use();
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
