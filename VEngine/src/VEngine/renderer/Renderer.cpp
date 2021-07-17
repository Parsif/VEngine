#include "precheader.h"
#include "Renderer.h"

#include "RenderCommand.h"

#include "MaterialLibrary.h"
#include "opengl/RendererApiGL.h"

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

		for (auto& shadow_map : m_dir_light_shadow_map_textures)
		{
			shadow_map.create(FrameBufferSpecifications{ 1024, 1024, FrameBufferType::DEPTH_ONLY });
		}
		m_environment_map_frame_buffer.create(FrameBufferSpecifications{ 1024, 1024, FrameBufferType::ENVIRONMENT_MAP });
		m_irradiance_map_frame_buffer.create(FrameBufferSpecifications{ 32, 32, FrameBufferType::ENVIRONMENT_MAP });

		m_pbr_render_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/pbr.vert", 
													"./VEngine/src/VEngine/renderer/shaders/pbr.frag", "PBR");

		m_hdr_to_cubemap_render_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/hdr_to_cubemap.vert", 
			"./VEngine/src/VEngine/renderer/shaders/hdr_to_cubemap.frag", "HDR_to_cubemap");

		m_skybox_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/skybox.vert",
			"./VEngine/src/VEngine/renderer/shaders/skybox.frag", "Skybox");
			
		m_direct_shadow_map_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/shadowmap.vert",
			"./VEngine/src/VEngine/renderer/shaders/shadowmap.frag", "Shadowmap");
		
		m_irradiance_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/irradiance.vert",
			"./VEngine/src/VEngine/renderer/shaders/irradiance.frag", "Irradiance");
		
		m_blur_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/blur.vert",
			"./VEngine/src/VEngine/renderer/shaders/blur.frag", "Blur");
		
		m_postprocessing_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/postprocessing.vert",
			"./VEngine/src/VEngine/renderer/shaders/postprocessing.frag", "Postprocessing");
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
		if(m_using_bloom)
		{
			m_intermediate_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::MULTI_TARGET, 4 });
			m_final_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::MULTI_TARGET, 1 });
			FrameBufferGL::set_draw_buffers(m_intermediate_frame_buffer.get_id(), 2);
		}
		else
		{
			m_intermediate_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL, 4 });
			m_final_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL, 1 });
			FrameBufferGL::set_draw_buffers(m_intermediate_frame_buffer.get_id(), 1);
		}
		
		//render shadowmaps
		const auto viewport = m_viewport;
		
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_shadow_map_specs.width, m_shadow_map_specs.height);
		//for every dir light render shadow map
		for(size_t i = 0; i < m_dir_lights.size(); ++i)
		{
			begin_render_pass(m_dir_light_shadow_map_textures[i]);
			for (auto& drawable : m_render_queue)
			{
				if (drawable.is_casting_shadow)
				{
					m_direct_shadow_map_material.set("u_transform", drawable.transform);
					const glm::mat4 light_space_matrix = calc_dir_light_space_matrix(m_dir_lights[i]);
					m_direct_shadow_map_material.set("u_light_space_matrix", light_space_matrix);
					m_pbr_render_material.set("u_dirlights[" + std::to_string(i) + "].light_space_matrix", light_space_matrix);
					m_direct_shadow_map_material.use();
					render_shadow(drawable);
				}
			}
			end_render_pass(m_dir_light_shadow_map_textures[i]);
		}
		
		//render scene
		RendererApiGL::set_viewport(viewport.x, viewport.y, viewport.width, viewport.height);
		
		begin_render_pass(m_intermediate_frame_buffer);
		for (auto& mesh : m_render_queue)
		{
			render_mesh(mesh);
		}
		end_render_pass(m_intermediate_frame_buffer);
		
		if(m_using_bloom)
		{
			//blitting brightness map
			FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_intermediate_frame_buffer.get_id(),
				m_final_frame_buffer.get_id(), 1);
			render_bloom();
		}
		
		if(m_using_environment_map)
		{
			m_intermediate_frame_buffer.bind();
			render_environment_map();
			m_intermediate_frame_buffer.unbind();
		}
		//blitting scene
		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_intermediate_frame_buffer.get_id(),
			m_final_frame_buffer.get_id(), 0);
		
		post_processing();
		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_intermediate_frame_buffer.get_id(), m_final_frame_buffer.get_id());
		
		m_render_queue.clear();
		destroy_lights();
	}


	void Renderer::set_viewport(int x, int y, unsigned int width, unsigned int height)
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
	}

	void Renderer::set_viewport(unsigned int width, unsigned int height)
	{
		m_viewport.width = width;
		m_viewport.height = height;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
	}

	void Renderer::add_dir_light(const DirLightComponent& dir_light, const glm::vec3& position)
	{
		if(m_dir_lights.size() == MAX_NUMBER_OF_DIR_LIGHTS)
		{
			return;
		}
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].position", position);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].color", dir_light.color);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].intensity", dir_light.intensity);
		m_dir_lights.push_back(DirLight{ dir_light, position });
	}

	void Renderer::add_point_light(const PointLightComponent& point_light, const glm::vec3& position)
	{
		if (m_point_lights.size() == MAX_NUMBER_OF_POINT_LIGHTS)
		{
			return;
		}
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].position", position);
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].color", point_light.color);
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].intensity", point_light.intensity);
		m_point_lights.push_back(PointLight{ point_light, position });
	}

	void Renderer::set_camera_params(const Camera& camera)
	{
		m_camera = camera;
		
		m_pbr_render_material.set("u_view_projection", m_camera.get_view_projection());
		m_pbr_render_material.set("u_view_pos", m_camera.get_position());

		m_skybox_material.set("u_view", m_camera.get_view());
		m_skybox_material.set("u_projection", m_camera.get_projection());
	}
		
	void Renderer::set_bloom(bool is_bloom_enabled)
	{
		m_using_bloom = is_bloom_enabled;
		m_postprocessing_material.set("u_is_bloom_enabled", is_bloom_enabled);
	}

	void Renderer::set_bloom_threshold(float threshold)
	{
		m_pbr_render_material.set("u_bloom_threshold", threshold);
	}

	void Renderer::set_bloom_intensity(float intensity)
	{
		m_pbr_render_material.set("u_bloom_intensity", intensity);
	}

	void Renderer::set_exposure(float exposure)
	{
		m_postprocessing_material.set("u_exposure", exposure);
	}

	void Renderer::destroy_lights()
	{
		m_dir_lights.clear();
		m_point_lights.clear();
	}

	void Renderer::set_scene_environment_map(const TextureGL& env_texture)
	{
		if(!env_texture)
		{
			m_using_environment_map = false;
			return;
		}
		m_using_environment_map = true;
		const glm::mat4 capture_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		glm::mat4 capture_views[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		
		// convert HDR equirectangular environment map to cubemap equivalent
		m_hdr_to_cubemap_render_material.set<int>("u_equirectangular_map", 0);
		env_texture.bind();
		m_hdr_to_cubemap_render_material.set("u_projection", capture_projection);
		
		const auto viewport = m_viewport;
		const auto& [width, height] = m_environment_map_frame_buffer.get_size();
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, width, height);
		begin_render_pass(m_environment_map_frame_buffer);
		for (unsigned int i = 0; i < 6; ++i)
		{
			m_hdr_to_cubemap_render_material.set("u_view", capture_views[i]);
			m_environment_map_frame_buffer.attach_cubemap_face_as_texture2d(i);
			m_hdr_to_cubemap_render_material.use();
			render_cube();
		}
		end_render_pass(m_environment_map_frame_buffer);

		//generate irradiance map
		m_irradiance_material.set<int>("u_environment_map", 0);
		m_environment_map_frame_buffer.bind_texture();
		m_irradiance_material.set("u_projection", capture_projection);

		const auto& [width2, height2] = m_irradiance_map_frame_buffer.get_size();
		RendererApiGL::set_viewport(viewport.x, viewport.y, width2, height2);
		begin_render_pass(m_irradiance_map_frame_buffer);
		for (unsigned int i = 0; i < 6; ++i)
		{
			m_irradiance_material.set("u_view", capture_views[i]);
			m_irradiance_map_frame_buffer.attach_cubemap_face_as_texture2d(i);
			m_irradiance_material.use();
			render_cube();
		}
		end_render_pass(m_irradiance_map_frame_buffer);
		RendererApiGL::set_viewport(viewport.x, viewport.y, viewport.width, viewport.height);
	}

	void Renderer::begin_render_pass(const FrameBufferGL& frame_buffer) const
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
		m_pbr_render_material.set<int>("u_number_of_dir_lights", m_dir_lights.size());
		m_pbr_render_material.set<int>("u_number_of_point_lights", m_point_lights.size());

		m_pbr_render_material.set("u_transform", mesh.transform);
		m_pbr_render_material.set("u_material.has_albedo", mesh.has_albedo_texture || mesh.materials.albedo_texture);
		m_pbr_render_material.set("u_material.has_metallic", mesh.has_metallic_texture || mesh.materials.metallic_texture);
		m_pbr_render_material.set("u_material.has_roughness", mesh.has_roughness_texture || mesh.materials.roughness_texture);
		m_pbr_render_material.set("u_material.has_ao", mesh.has_ao_texture || mesh.materials.ao_texture);
		m_pbr_render_material.set("u_material.has_normal_map", mesh.has_normal_texture || mesh.materials.normal_texture);

		unsigned int texture_slot = 0;
		m_pbr_render_material.set<int>("u_irradiance_map", texture_slot);
		m_irradiance_map_frame_buffer.bind_texture(texture_slot++);
		
		for (size_t i = 0; i < m_dir_lights.size(); ++i)
		{
			m_pbr_render_material.set<int>("u_dirlights[" + std::to_string(i) + "].shadow_map", texture_slot);
			m_dir_light_shadow_map_textures[i].bind_texture(texture_slot++);
		}
		
		for (auto&& render_command : mesh.commands)
		{
			if (mesh.materials.albedo_texture)
			{
				m_pbr_render_material.set<int>("u_material." + mesh.materials.albedo_texture.get_string_type(), texture_slot);
				mesh.materials.albedo_texture.bind(texture_slot++);
			}
			if (mesh.materials.metallic_texture)
			{
				m_pbr_render_material.set<int>("u_material." + mesh.materials.metallic_texture.get_string_type(), texture_slot);
				mesh.materials.metallic_texture.bind(texture_slot++);
			}
			if (mesh.materials.roughness_texture)
			{
				m_pbr_render_material.set<int>("u_material." + mesh.materials.roughness_texture.get_string_type(), texture_slot);
				mesh.materials.roughness_texture.bind(texture_slot++);
			}
			if (mesh.materials.ao_texture)
			{
				m_pbr_render_material.set<int>("u_material." + mesh.materials.ao_texture.get_string_type(), texture_slot);
				mesh.materials.ao_texture.bind(texture_slot++);
			}
			if (mesh.materials.normal_texture)
			{
				m_pbr_render_material.set<int>("u_material." + mesh.materials.normal_texture.get_string_type(), texture_slot);
				mesh.materials.normal_texture.bind(texture_slot++);
			}
			
			for (const auto& texture_2d : render_command.get_textures2d())
			{
				m_pbr_render_material.set<int>("u_material." + texture_2d.get_string_type(), texture_slot);
				texture_2d.bind(texture_slot++);
			}
			
			m_pbr_render_material.use();
			RendererApiGL::draw_elements(render_command);
			texture_slot = 0;
		}
	}

	void Renderer::render_shadow(Mesh& mesh) const
	{
		for (auto&& render_command : mesh.commands)
		{
			RendererApiGL::draw_elements(render_command);
		}
	}

	void Renderer::render_bloom()
	{
		bool is_bloom_horizontal = true;
		std::array<FrameBufferGL, 2> pingpong_fbos;
		
		for (auto& frame_buffer : pingpong_fbos)
		{
			frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_ONLY });
		}
		m_blur_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_ONLY });
		
		const int BLUR_CYCLES = 10;
		for (size_t i = 0; i < BLUR_CYCLES; ++i)
		{
			begin_render_pass(pingpong_fbos[is_bloom_horizontal]);
			m_blur_material.set<int>("u_image", 0);
			if(i == 0)
			{
				m_final_frame_buffer.bind_texture(0, 1);
			}
			else
			{
				pingpong_fbos[!is_bloom_horizontal].bind_texture();
			}
			m_blur_material.set("u_horizontal", is_bloom_horizontal);
			m_blur_material.use();
			render_quad();
			end_render_pass(pingpong_fbos[is_bloom_horizontal]);

			is_bloom_horizontal = !is_bloom_horizontal;
		}
		
		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, pingpong_fbos[!is_bloom_horizontal].get_id(), m_blur_frame_buffer.get_id());
	}

	void Renderer::post_processing()
	{
		begin_render_pass(m_intermediate_frame_buffer);
		m_postprocessing_material.set<int>("u_scene", 0);
		m_postprocessing_material.set<int>("u_bloom_blur", 1);
		m_postprocessing_material.set("u_is_bloom_enabled", m_using_bloom);
		m_final_frame_buffer.bind_texture(0, 0);
		if(m_using_bloom)
		{
			m_blur_frame_buffer.bind_texture(1);
		}
		m_postprocessing_material.use();
		render_quad();
		end_render_pass(m_intermediate_frame_buffer);
	}

	void Renderer::render_environment_map() 
	{
		m_skybox_material.set<int>("u_environment_map", 0);
		m_environment_map_frame_buffer.bind_texture();
		m_skybox_material.use();
		render_cube();
	}

	void Renderer::render_cube() const
	{
		RenderCommand render_command;
		float vertices[8 * 3] =
		{
			-1, -1, -1,
			1, -1, -1,
			1, 1, -1,
			-1, 1, -1,
			-1, -1, 1,
			1, -1, 1,
			1, 1, 1,
			-1, 1, 1
		};

		unsigned int indices[6 * 6] =
		{
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		render_command.set_vertex_buffer(vertices, sizeof(vertices));
		render_command.set_index_buffer(indices, sizeof(indices), sizeof(indices) / sizeof(unsigned int));
		render_command.set_buffer_layout(BufferLayout{ ShaderDataType::Float3 });

		RendererApiGL::draw_elements(render_command);
	}

	void Renderer::render_quad() const
	{
		RenderCommand render_command;
		float vertices[4 * 5] =
		{
			 -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		render_command.set_vertex_buffer(vertices, sizeof(vertices));
		render_command.set_buffer_layout(BufferLayout{ ShaderDataType::Float3, ShaderDataType::Float2 });
		render_command.set_primitive_type(PrimitiveType::TRIANGLE_STRIP);
		
		RendererApiGL::draw_arrays(render_command, 0, 4);
	}

	[[nodiscard]] glm::mat4 Renderer::calc_dir_light_space_matrix(const DirLight& dir_light) const
	{
		const float near_plane = 1.0f, far_plane = 50.0f;
		const glm::mat4 light_projection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);

		glm::mat4 light_view = glm::lookAt(dir_light.position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		return light_projection * light_view;
	}
}
