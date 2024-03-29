#pragma once

#include "Mesh.h"
#include "RenderCommand.h"

#include "RenderPassDescriptor.h"
#include "opengl/FrameBufferGL.h"
#include "scene/Components.h"

namespace vengine
{
	class Renderer
	{
	private:
		template<class T>
		struct Light
		{
			T light_component;
			glm::vec3 position;
		};
		
		struct Viewport
		{
			int x = 0;
			int y = 0;
			unsigned int width = 0;
			unsigned int height = 0;
		};
		
	public:
		void init(unsigned int width, unsigned int height);
		void shutdown();

		void add_drawable(const Mesh& drawable);

		void render();
		void set_viewport(int x, int y, unsigned int width, unsigned int height);
		void set_viewport(unsigned int width, unsigned int height);
		void add_dir_light(const DirLightComponent& dir_light, const glm::vec3& position);
		void add_point_light(const PointLightComponent& point_light, const glm::vec3& position);
		void add_sphere_area_light(const SphereAreaLightComponent& sphere_area_light_component, const glm::vec3& position);
		void add_tube_area_light(const TubeAreaLightComponent& sphere_area_light_component, const glm::vec3& position);
		
		void set_camera_params(const Camera& camera);
		void set_exposure(float exposure);

		[[nodiscard]] auto get_color_attachment() const { return m_final_frame_buffer.get_color_attachment0(); }
		
		[[nodiscard]] auto get_viewport() const { return m_viewport; }
		void destroy_lights();
		void set_scene_environment_map(const TextureGL& env_texture);

		[[nodiscard]] static unsigned int get_max_lights() { return MAX_LIGHTS; }

	private:
		void create_resources();
		void begin_render_pass(const FrameBufferGL& frame_buffer) const;
		void end_render_pass(const FrameBufferGL& frame_buffer) const;
		void render_scene();
		void gbuffer_pass();
		void lightning_pass();
		void render_shadows();
		void render_shadow(Mesh& mesh) const;
		void render_bloom();
		void post_processing();

		void render_environment_map();
		void render_cube() const;
		void render_quad() const;

		void set_dir_light_space_matrix(const Light<DirLightComponent>& dir_light, int light_index);
		void set_point_light_space_matrices(const Light<PointLightComponent>& point_light, int light_index);

	
	private:
		Viewport m_viewport;
		
		std::vector<Mesh> m_render_queue;
		RenderPassDescriptor m_render_pass_descriptor;

		FrameBufferGL m_intermediate_frame_buffer;
		FrameBufferGL m_final_frame_buffer;
		FrameBufferGL m_environment_map_frame_buffer;
		FrameBufferGL m_irradiance_map_frame_buffer;
		FrameBufferGL m_prefilter_map_frame_buffer;
		FrameBufferGL m_convolute_brdf_map_frame_buffer;

		FrameBufferGL m_gbuffer;

		const static unsigned int MAX_LIGHTS = 4;
		std::array<FrameBufferGL, MAX_LIGHTS> m_dir_light_shadow_map_textures{};
		std::array<FrameBufferGL, MAX_LIGHTS> m_point_light_shadow_map_textures{};

		const FrameBufferSpecifications m_shadow_map_specs{  };

		std::vector<Light<DirLightComponent>> m_dir_lights;
		std::vector<Light<PointLightComponent>> m_point_lights;
		std::vector<Light<SphereAreaLightComponent>> m_sphere_area_lights;
		std::vector<Light<TubeAreaLightComponent>> m_tube_area_lights;

		Material m_pbr_render_material;
		Material m_hdr_to_cubemap_render_material;
		Material m_direct_shadow_map_material;
		Material m_omnidirect_shadow_map_material;
		Material m_skybox_material;
		Material m_irradiance_material;
		Material m_prefilter_material;
		Material m_convolute_brdf_material;
		Material m_blur_material;
		Material m_postprocessing_material;
		Material m_geometry_pass_material;

		ComputeShader m_downsample_compute_shader;
		ComputeShader m_lightmap_compute_shader;

		Camera m_camera;

		bool m_using_environment_map = false;
		bool m_using_bloom = false;
	};
	
}



