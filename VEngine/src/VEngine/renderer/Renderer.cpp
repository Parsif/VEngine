#include "precheader.h"
#include "Renderer.h"

#include "RenderCommand.h"

#include "MaterialLibrary.h"
#include "opengl/RendererApiGL.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


namespace vengine
{
	void Renderer::init(uint32_t width, uint32_t height)
	{
		m_render_pass_descriptor.depth_test_enabled = true;
		m_render_pass_descriptor.need_clear_color = true;
		m_render_pass_descriptor.need_clear_depth = true;
		m_render_pass_descriptor.need_culling = true;

		m_viewport.x = 0;
		m_viewport.y = 0;
		m_viewport.width = width;
		m_viewport.height = height;
		create_resources();

		for (auto& shadow_map : m_dir_light_shadow_map_textures)
		{
			shadow_map.create(FrameBufferSpecifications{ 1024, 1024, FrameBufferType::DEPTH_ONLY });
		}
		
		for (auto& shadow_map : m_point_light_shadow_map_textures)
		{
			shadow_map.create(FrameBufferSpecifications{ 1024, 1024, FrameBufferType::CUBE_MAP_DEPTH_ONLY });
		}
		
		m_environment_map_frame_buffer.create(FrameBufferSpecifications{ 1024, 1024, FrameBufferType::CUBE_MAP });
		m_irradiance_map_frame_buffer.create(FrameBufferSpecifications{ 32, 32, FrameBufferType::CUBE_MAP });
		m_prefilter_map_frame_buffer.create(FrameBufferSpecifications{ 128, 128, FrameBufferType::CUBE_MAP, 1, true });
		m_convolute_brdf_map_frame_buffer.create(FrameBufferSpecifications{ 512, 512, FrameBufferType::TWO_CHANNELS_ONLY });
		
		m_pbr_render_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/pbr.vert", 
													"./VEngine/src/VEngine/renderer/shaders/pbr.frag", "PBR");

		m_hdr_to_cubemap_render_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/hdr_to_cubemap.vert", 
			"./VEngine/src/VEngine/renderer/shaders/hdr_to_cubemap.frag", "HDR_to_cubemap");

		m_skybox_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/skybox.vert",
			"./VEngine/src/VEngine/renderer/shaders/skybox.frag", "Skybox");
			
		m_direct_shadow_map_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/shadowmap.vert",
			"./VEngine/src/VEngine/renderer/shaders/shadowmap.frag", "Direct shadowmap");

		m_omnidirect_shadow_map_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/omnidirect_shadowmap.vert",
			"./VEngine/src/VEngine/renderer/shaders/omnidirect_shadowmap.frag", 
			"./VEngine/src/VEngine/renderer/shaders/omnidirect_shadowmap.geom", "Omnidirect shadowmap");
		
		m_irradiance_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/irradiance.vert",
			"./VEngine/src/VEngine/renderer/shaders/irradiance.frag", "Irradiance");
		
		m_prefilter_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/prefilter.vert",
			"./VEngine/src/VEngine/renderer/shaders/prefilter.frag", "Prefilter");
		
		m_convolute_brdf_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/convolute_brdf.vert",
			"./VEngine/src/VEngine/renderer/shaders/convolute_brdf.frag", "Convolute BRDF");
		
		m_postprocessing_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/postprocessing.vert",
			"./VEngine/src/VEngine/renderer/shaders/postprocessing.frag", "Postprocessing");

		m_lightmap_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/lightmap.comp", "Lightmap");
		m_downsample_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/downsample.comp", "Downsample");

		m_geometry_pass_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/geometry_pass.vert",
			"./VEngine/src/VEngine/renderer/shaders/geometry_pass.frag", "GeometryPass");

		m_blur_compute_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/blur.comp", "ComputeBlur");

		m_blend_compute_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/blend.comp", "ComputeBlend");

		m_upsample_compute_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/upsample.comp", "ComputeUpsample");

		m_volumetric_light_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/volumetric_light.vert",
			"./VEngine/src/VEngine/renderer/shaders/volumetric_light.frag", "VolumetricLight");

		m_taa_resolve_material = MaterialLibrary::load("./VEngine/src/VEngine/renderer/shaders/taa_resolve.vert", 
			"./VEngine/src/VEngine/renderer/shaders/taa_resolve.frag", "TAA_Resolve");
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
		m_final_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL });


		render_shadows();
		render_scene();
		if (m_using_bloom)
		{
			render_bloom();
		}

		if (m_using_environment_map)
		{
			render_environment_map();
		}
		post_processing();

		m_render_queue.clear();
		destroy_lights();
	}

	void Renderer::set_viewport(int x, int y, uint32_t width, uint32_t height)
	{
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
	
		create_resources();
	}

	void Renderer::set_viewport(uint32_t width, uint32_t height)
	{
		m_viewport.width = width;
		m_viewport.height = height;
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
		create_resources();
	}

	void Renderer::add_dir_light(const DirLightComponent& dir_light, const glm::vec3& position)
	{
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].position", position);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].color", dir_light.color);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].intensity", dir_light.intensity);

		m_volumetric_light_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].position", position);
		m_volumetric_light_material.set("u_dirlights[" + std::to_string(m_dir_lights.size()) + "].color", dir_light.color);

		m_dir_lights.push_back(Light<DirLightComponent>{ dir_light, position });
	}

	void Renderer::add_point_light(const PointLightComponent& point_light, const glm::vec3& position)
	{
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].position", position);
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].color", point_light.color);
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].intensity", point_light.intensity);
		m_pbr_render_material.set("u_point_lights[" + std::to_string(m_point_lights.size()) + "].light_radius", point_light.light_radius);
		m_point_lights.push_back(Light<PointLightComponent>{ point_light, position });
	}

	void Renderer::add_sphere_area_light(const SphereAreaLightComponent& sphere_area_light,
		const glm::vec3& position)
	{
		m_pbr_render_material.set("u_sphere_area_lights[" + std::to_string(m_point_lights.size()) + "].position", position);
		m_pbr_render_material.set("u_sphere_area_lights[" + std::to_string(m_point_lights.size()) + "].color", sphere_area_light.color);
		m_pbr_render_material.set("u_sphere_area_lights[" + std::to_string(m_point_lights.size()) + "].intensity", sphere_area_light.intensity);
		m_pbr_render_material.set("u_sphere_area_lights[" + std::to_string(m_point_lights.size()) + "].light_radius", sphere_area_light.light_radius);
		m_pbr_render_material.set("u_sphere_area_lights[" + std::to_string(m_point_lights.size()) + "].source_radius", sphere_area_light.source_radius);

		m_sphere_area_lights.push_back(Light<SphereAreaLightComponent>{ sphere_area_light, position });
	}

	void Renderer::add_tube_area_light(const TubeAreaLightComponent& tube_area_light,
		const glm::vec3& position)
	{
		m_pbr_render_material.set("u_tube_area_lights[" + std::to_string(m_tube_area_lights.size()) + "].position1", 
			glm::vec3(position.x, position.y + tube_area_light.length, position.z));
		m_pbr_render_material.set("u_tube_area_lights[" + std::to_string(m_tube_area_lights.size()) + "].position2",
			glm::vec3(position.x, position.y - tube_area_light.length, position.z));
		m_pbr_render_material.set("u_tube_area_lights[" + std::to_string(m_tube_area_lights.size()) + "].color", tube_area_light.color);
		m_pbr_render_material.set("u_tube_area_lights[" + std::to_string(m_tube_area_lights.size()) + "].intensity", tube_area_light.intensity);
		m_pbr_render_material.set("u_tube_area_lights[" + std::to_string(m_tube_area_lights.size()) + "].light_radius", tube_area_light.light_radius);
		m_pbr_render_material.set("u_tube_area_lights[" + std::to_string(m_tube_area_lights.size()) + "].source_radius", tube_area_light.source_radius);

		m_tube_area_lights.push_back(Light<TubeAreaLightComponent>{ tube_area_light, position });
	}

	void Renderer::set_camera_params(const Camera& camera, glm::mat4 prev_frame_view_projection)
	{
		m_camera = camera;
		
		m_pbr_render_material.set("u_view_pos", m_camera.get_position());
		m_volumetric_light_material.set("u_view_pos", m_camera.get_position());

		m_geometry_pass_material.set("u_view_projection", m_camera.get_view_projection());
		m_geometry_pass_material.set("u_previous_frame_view_projection", prev_frame_view_projection);

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
		m_lightmap_material.set("u_bloom_threshold", threshold);
	}

	void Renderer::set_bloom_intensity(float intensity)
	{
		m_lightmap_material.set("u_bloom_intensity", intensity);
	}

	void Renderer::set_exposure(float exposure)
	{
		m_postprocessing_material.set("u_exposure", exposure);
	}

	void Renderer::destroy_lights()
	{
		for (size_t i = 0; i < m_dir_lights.size(); ++i)
		{
			m_pbr_render_material.set<int>("u_dirlights[" + std::to_string(i) + "].shadow_map", 0);
		}

		for (size_t i = 0; i < m_point_lights.size(); ++i)
		{
			m_pbr_render_material.set<int>("u_point_lights[" + std::to_string(i) + "].shadow_map", 0);
		}

		m_pbr_render_material.set<int>("u_irradiance_map", 0);
		m_pbr_render_material.set<int>("u_prefilter_map", 0);
		m_pbr_render_material.set<int>("u_convoluted_brdf", 0);
		
		m_dir_lights.clear();
		m_point_lights.clear();
		m_sphere_area_lights.clear();
		m_tube_area_lights.clear();
	}

	void Renderer::set_scene_environment_map(const TextureGL& env_texture)
	{
		m_using_environment_map = (bool)env_texture;
		m_pbr_render_material.set("u_using_environment_map", m_using_environment_map);
		
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
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_environment_map_frame_buffer.get_width(), m_environment_map_frame_buffer.get_height());
		begin_render_pass(m_environment_map_frame_buffer);
		for (uint32_t i = 0; i < 6; ++i)
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

		RendererApiGL::set_viewport(viewport.x, viewport.y, m_irradiance_map_frame_buffer.get_width(), m_environment_map_frame_buffer.get_height());
		begin_render_pass(m_irradiance_map_frame_buffer);
		for (uint32_t i = 0; i < 6; ++i)
		{
			m_irradiance_material.set("u_view", capture_views[i]);
			m_irradiance_map_frame_buffer.attach_cubemap_face_as_texture2d(i);
			m_irradiance_material.use();
			render_cube();
		}
		end_render_pass(m_irradiance_map_frame_buffer);

		
		//generate prefilter map
		m_prefilter_material.set("u_projection", capture_projection);
		m_prefilter_material.set<int>("u_environment_map", 0);
		m_environment_map_frame_buffer.bind_texture();

		begin_render_pass(m_prefilter_map_frame_buffer);
		uint32_t maxMipLevels = 5;
		for (uint32_t mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			uint32_t mipWidth = 128 * std::pow(0.5, mip);
			uint32_t mipHeight = 128 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, m_prefilter_map_frame_buffer.get_rbo());
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			RendererApiGL::set_viewport(viewport.x, viewport.y, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);

			m_prefilter_material.set<float>("u_roughness", roughness);
			for (uint32_t i = 0; i < 6; ++i)
			{
				m_prefilter_material.set("u_view", capture_views[i]);
				m_prefilter_map_frame_buffer.attach_cubemap_face_as_texture2d(i);
				m_prefilter_material.use();
				render_cube();
			}
		}
		end_render_pass(m_irradiance_map_frame_buffer);

		//convolute brdf on 2d texture
		RendererApiGL::set_viewport(viewport.x, viewport.y, m_convolute_brdf_map_frame_buffer.get_width(), m_convolute_brdf_map_frame_buffer.get_height());
		begin_render_pass(m_convolute_brdf_map_frame_buffer);
		m_convolute_brdf_material.use();
		render_quad();
		end_render_pass(m_convolute_brdf_map_frame_buffer);
		
		
		RendererApiGL::set_viewport(viewport.x, viewport.y, viewport.width, viewport.height);
	}

	void Renderer::create_resources()
	{
		uint32_t downsample_factor = 1;
		for (auto& texture : m_light_mipmap_textures)
		{
			texture.create(m_viewport.width / downsample_factor, m_viewport.height / downsample_factor);
			downsample_factor *= 2;
		}

		m_gbuffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::G_BUFFER });
		m_intermediate_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL });
		m_previous_frame_buffer.create(FrameBufferSpecifications{ m_viewport.width, m_viewport.height, FrameBufferType::COLOR_DEPTH_STENCIL });
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

	void Renderer::render_scene()
	{
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_final_frame_buffer.get_width(), m_final_frame_buffer.get_height());

		uint32_t attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glNamedFramebufferDrawBuffers(m_gbuffer.get_id(), sizeof(attachments) / sizeof(uint32_t), attachments);
		begin_render_pass(m_gbuffer);
		gbuffer_pass();
		end_render_pass(m_gbuffer);

		begin_render_pass(m_final_frame_buffer);
		lightning_pass();
		end_render_pass(m_final_frame_buffer);
	}

	void Renderer::gbuffer_pass()
	{
		for (auto& mesh : m_render_queue)
		{
			m_geometry_pass_material.set("u_transform", mesh.transform);
			m_geometry_pass_material.set("u_prev_frame_transform", mesh.prev_frame_transform);
			m_geometry_pass_material.set("u_material.has_albedo", mesh.has_albedo_texture || mesh.materials.albedo_texture);
			m_geometry_pass_material.set("u_material.has_metallic", mesh.has_metallic_texture || mesh.materials.metallic_texture);
			m_geometry_pass_material.set("u_material.has_roughness", mesh.has_roughness_texture || mesh.materials.roughness_texture);
			m_geometry_pass_material.set("u_material.has_ao", mesh.has_ao_texture || mesh.materials.ao_texture);
			m_geometry_pass_material.set("u_material.has_normal_map", mesh.has_normal_texture || mesh.materials.normal_texture);

			for (auto&& render_command : mesh.commands)
			{
				int texture_slot = 1;
				if (mesh.materials.albedo_texture)
				{
					m_geometry_pass_material.set<int>("u_material." + mesh.materials.albedo_texture.get_string_type(), texture_slot);
					mesh.materials.albedo_texture.bind(texture_slot++);
				}
				if (mesh.materials.metallic_texture)
				{
					m_geometry_pass_material.set<int>("u_material." + mesh.materials.metallic_texture.get_string_type(), texture_slot);
					mesh.materials.metallic_texture.bind(texture_slot++);
				}
				if (mesh.materials.roughness_texture)
				{
					m_geometry_pass_material.set<int>("u_material." + mesh.materials.roughness_texture.get_string_type(), texture_slot);
					mesh.materials.roughness_texture.bind(texture_slot++);
				}
				if (mesh.materials.ao_texture)
				{
					m_geometry_pass_material.set<int>("u_material." + mesh.materials.ao_texture.get_string_type(), texture_slot);
					mesh.materials.ao_texture.bind(texture_slot++);
				}
				if (mesh.materials.normal_texture)
				{
					m_geometry_pass_material.set<int>("u_material." + mesh.materials.normal_texture.get_string_type(), texture_slot);
					mesh.materials.normal_texture.bind(texture_slot++);
				}

				for (const auto& texture_2d : render_command.get_textures2d())
				{
					m_geometry_pass_material.set<int>("u_material." + texture_2d.get_string_type(), texture_slot);
					texture_2d.bind(texture_slot++);
				}

				m_geometry_pass_material.use();
				RendererApiGL::draw_elements(render_command);
			}
		}
	}

	void Renderer::lightning_pass() 
	{
		m_pbr_render_material.set<int>("u_number_of_dir_lights", m_dir_lights.size());
		m_pbr_render_material.set<int>("u_number_of_point_lights", m_point_lights.size());
		m_pbr_render_material.set<int>("u_number_of_sphere_area_lights", m_sphere_area_lights.size());
		m_pbr_render_material.set<int>("u_number_of_tube_area_lights", m_tube_area_lights.size());

		int texture_slot = 1;

		if(m_using_environment_map)
		{
			m_pbr_render_material.set<int>("u_irradiance_map", texture_slot);
			m_irradiance_map_frame_buffer.bind_texture(texture_slot++);

			m_pbr_render_material.set<int>("u_prefilter_map", texture_slot);
			m_prefilter_map_frame_buffer.bind_texture(texture_slot++);

			m_pbr_render_material.set<int>("u_convoluted_brdf", texture_slot);
			m_convolute_brdf_map_frame_buffer.bind_texture(texture_slot++);
		}
	
		for (size_t i = 0; i < m_dir_lights.size(); ++i)
		{
			m_pbr_render_material.set<int>("u_dirlights[" + std::to_string(i) + "].shadow_map", texture_slot);
			m_dir_light_shadow_map_textures[i].bind_texture(texture_slot++);
		}

		for (size_t i = 0; i < m_point_lights.size(); ++i)
		{
			m_pbr_render_material.set<int>("u_point_lights[" + std::to_string(i) + "].shadow_map", texture_slot);
			m_point_light_shadow_map_textures[i].bind_texture(texture_slot++);
		}
		
		m_pbr_render_material.set<int>("u_gbuffer.position_map", texture_slot);
		m_gbuffer.bind_texture(texture_slot++, 0);
		m_pbr_render_material.set<int>("u_gbuffer.albedo_map", texture_slot);
		m_gbuffer.bind_texture(texture_slot++, 1);
		m_pbr_render_material.set<int>("u_gbuffer.normal_map", texture_slot);
		m_gbuffer.bind_texture(texture_slot++, 2);
		m_pbr_render_material.set<int>("u_gbuffer.metallic_roughness_ao_map", texture_slot);
		m_gbuffer.bind_texture(texture_slot++, 3);

		m_pbr_render_material.set<int>("u_depth_buffer", texture_slot);
		glActiveTexture(GL_TEXTURE0 + texture_slot++);
		glBindTexture(GL_TEXTURE_2D, m_gbuffer.get_depth_stencil_attachment());
			
		m_pbr_render_material.use();
		render_quad();
	}

	void Renderer::render_shadows()
	{
		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_dir_light_shadow_map_textures[0].get_width(), m_dir_light_shadow_map_textures[0].get_height());
		//for every dir light render shadow map
		for (size_t i = 0; i < m_dir_lights.size(); ++i)
		{
			begin_render_pass(m_dir_light_shadow_map_textures[i]);
			for (auto& drawable : m_render_queue)
			{
				if (drawable.is_casting_shadow)
				{
					m_direct_shadow_map_material.set("u_transform", drawable.transform);
					set_dir_light_space_matrix(m_dir_lights[i], i);
					m_direct_shadow_map_material.use();
					render_shadow(drawable);
				}
			}
			end_render_pass(m_dir_light_shadow_map_textures[i]);
		}

		RendererApiGL::set_viewport(m_viewport.x, m_viewport.y, m_point_light_shadow_map_textures[0].get_width(), m_point_light_shadow_map_textures[0].get_height());
		//for every dir light render shadow map
		for (size_t i = 0; i < m_point_lights.size(); ++i)
		{
			begin_render_pass(m_point_light_shadow_map_textures[i]);
			for (auto& drawable : m_render_queue)
			{
				if (drawable.is_casting_shadow)
				{
					m_omnidirect_shadow_map_material.set("u_transform", drawable.transform);
					m_omnidirect_shadow_map_material.set("u_light_pos", m_point_lights[i].position);
				    set_point_light_space_matrices(m_point_lights[i], i);
					m_omnidirect_shadow_map_material.use();
					render_shadow(drawable);
				}
			}
			end_render_pass(m_point_light_shadow_map_textures[i]);
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
		glBindImageTexture(0, m_final_frame_buffer.get_color_attachment0(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
		glBindImageTexture(1, m_light_mipmap_textures[0].get_id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
		m_lightmap_material.use();
		RendererApiGL::dispatch_compute(64, 64, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		for (size_t i = 1; i < m_light_mipmap_textures.size(); ++i)
		{
			glBindImageTexture(0, m_light_mipmap_textures[i - 1].get_id(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
			glBindImageTexture(1, m_light_mipmap_textures[i].get_id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
			m_downsample_material.use();
			RendererApiGL::dispatch_compute(64, 64, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

		for (size_t i = 0; i < m_light_mipmap_textures.size(); ++i)
		{
			blur_texture(m_light_mipmap_textures[i]);
		}

		for (size_t i = m_light_mipmap_textures.size() - 1; i > 0; --i)
		{
			// blend(i, i - 1);
		}
	}

	void Renderer::post_processing()
	{
		//volumetric_light();
		
		//////////TAA resolve///////////
	/*	begin_render_pass(m_intermediate_frame_buffer);
		m_taa_resolve_material.set<int>("u_current_frame", 0);
		m_final_frame_buffer.bind_texture(0);
		m_taa_resolve_material.set<int>("u_prev_frame", 1);
		m_previous_frame_buffer.bind_texture(0);
		m_taa_resolve_material.set<int>("u_velocity_buffer", 2);
		m_gbuffer.bind_texture(0, 4);
		m_taa_resolve_material.use();
		render_quad();
		end_render_pass(m_intermediate_frame_buffer);
		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_intermediate_frame_buffer.get_id(), m_final_frame_buffer.get_id());*/
		////////////////////////////////

		begin_render_pass(m_intermediate_frame_buffer);
		m_postprocessing_material.set<int>("u_scene", 1);
		m_final_frame_buffer.bind_texture(1);
		m_postprocessing_material.set<int>("u_bloom_blur", 0);
		m_light_mipmap_textures[0].bind(0);
		m_postprocessing_material.use();
		render_quad();
		end_render_pass(m_intermediate_frame_buffer);

		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_intermediate_frame_buffer.get_id(), m_final_frame_buffer.get_id());
		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_final_frame_buffer.get_id(), m_previous_frame_buffer.get_id());
	}

	void Renderer::volumetric_light()
	{
		uint32_t texture_slot = 0;
		begin_render_pass(m_intermediate_frame_buffer);
		m_volumetric_light_material.set<int>("u_scene", texture_slot);
		m_final_frame_buffer.bind_texture(texture_slot++);
		m_volumetric_light_material.set<int>("u_position_map", texture_slot);
		m_gbuffer.bind_texture(texture_slot++, 0);
		for (size_t i = 0; i < m_dir_lights.size(); ++i)
		{
			m_volumetric_light_material.set<int>("u_dirlights[" + std::to_string(i) + "].shadow_map", texture_slot);
			m_dir_light_shadow_map_textures[i].bind_texture(texture_slot++);
		}

		m_volumetric_light_material.use();
		render_quad();
		end_render_pass(m_intermediate_frame_buffer);

		FrameBufferGL::blit_framebuffer(m_viewport.width, m_viewport.height, m_intermediate_frame_buffer.get_id(), m_final_frame_buffer.get_id());
	}

	void Renderer::blur_texture(const TextureGL& texture)
	{
		for (size_t i = 0; i < 1; ++i)
		{
			glBindImageTexture(0, texture.get_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
			m_blur_compute_material.set("u_horizontal", false);
			m_blur_compute_material.use();
			RendererApiGL::dispatch_compute(64, 64, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			glBindImageTexture(0, texture.get_id(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
			m_blur_compute_material.set("u_horizontal", true);
			m_blur_compute_material.use();
			RendererApiGL::dispatch_compute(64, 64, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
		
	}

	void Renderer::render_environment_map() 
	{
		m_final_frame_buffer.bind();
		m_skybox_material.set<int>("u_environment_map", 0);
		m_environment_map_frame_buffer.bind_texture();
		m_skybox_material.use();
		render_cube();
		m_final_frame_buffer.unbind();
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

		uint32_t indices[6 * 6] =
		{
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		render_command.set_vertex_buffer(vertices, sizeof(vertices));
		render_command.set_index_buffer(indices, sizeof(indices), sizeof(indices) / sizeof(uint32_t));
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

	void Renderer::set_dir_light_space_matrix(const Light<DirLightComponent>& dir_light, int light_index)
	{
		const float near_plane = 1.0f, far_plane = 50.0f;
		const glm::mat4 light_projection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

		glm::mat4 light_space_matrix = light_projection * glm::lookAt(dir_light.position,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		
		m_direct_shadow_map_material.set("u_light_space_matrix", light_space_matrix);
		m_pbr_render_material.set("u_dirlights[" + std::to_string(light_index) + "].light_space_matrix", light_space_matrix);
		m_volumetric_light_material.set("u_dirlights[" + std::to_string(light_index) + "].light_space_matrix", light_space_matrix);
	}

	void Renderer::set_point_light_space_matrices(const Light<PointLightComponent>& point_light, int light_index)
	{
		const float NEAR_PLANE = 1.0f;
		const float FAR_PLANE = 25.0f;
		const float aspect_ratio = (float)m_point_light_shadow_map_textures[0].get_width() / (float)m_point_light_shadow_map_textures[0].get_height();
		const glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect_ratio, NEAR_PLANE, FAR_PLANE);
		std::vector<glm::mat4> view_projections;
		view_projections.push_back(projection *
			glm::lookAt(point_light.position, point_light.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		view_projections.push_back(projection *
			glm::lookAt(point_light.position, point_light.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		view_projections.push_back(projection *
			glm::lookAt(point_light.position, point_light.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		view_projections.push_back(projection *
			glm::lookAt(point_light.position, point_light.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		view_projections.push_back(projection *
			glm::lookAt(point_light.position, point_light.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		view_projections.push_back(projection *
			glm::lookAt(point_light.position, point_light.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		for (size_t i = 0; i < view_projections.size(); ++i)
		{
			m_omnidirect_shadow_map_material.set("uniforms[" + std::to_string(i) + "].light_space_transforms", view_projections[i]);
		}
		
		m_pbr_render_material.set<float>("u_far_plane", FAR_PLANE);
		m_omnidirect_shadow_map_material.set<float>("u_far_plane", FAR_PLANE);
	}
}
