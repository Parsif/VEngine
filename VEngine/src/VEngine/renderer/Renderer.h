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
		struct DirLight
		{
			DirLightComponent dir_light_component;
			glm::vec3 position;
		};

		struct PointLight
		{
			PointLightComponent point_light_component;
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
		void init();
		void shutdown();

		void add_drawable(const Mesh& drawable);

		void render();
		void set_viewport(int x, int y, unsigned int width, unsigned int height);
		void set_viewport(unsigned int width, unsigned int height);
		void add_dir_light(const DirLightComponent& dir_light, const glm::vec3& position);
		void add_point_light(const PointLightComponent& point_light, const glm::vec3& position);

		void set_camera_params(const Camera& camera);
		void set_bloom(bool is_bloom_enabled);
		void set_bloom_threshold(float threshold);
		void set_bloom_intensity(float intensity);
		void set_exposure(float exposure);

		[[nodiscard]] auto get_color_attachment() const { return m_final_frame_buffer.get_color_attachment0(); }
		

		[[nodiscard]] auto get_viewport() const { return m_viewport; }
		void destroy_lights();
		void set_scene_environment_map(const TextureGL& env_texture);

	private:
		void begin_render_pass(const FrameBufferGL& frame_buffer) const;
		void end_render_pass(const FrameBufferGL& frame_buffer) const;
		void render_mesh(Mesh& mesh);
		void render_shadow(Mesh& mesh) const;
		void render_bloom();
		void post_processing();

		void render_environment_map();
		void render_cube() const;
		void render_quad() const;


		[[nodiscard]] glm::mat4 calc_dir_light_space_matrix(const DirLight& dir_light) const;

	
	private:
		Viewport m_viewport;
		
		std::vector<Mesh> m_render_queue;
		RenderPassDescriptor m_render_pass_descriptor;

		FrameBufferGL m_intermediate_frame_buffer;
		FrameBufferGL m_final_frame_buffer;
		FrameBufferGL m_environment_map_frame_buffer;
		FrameBufferGL m_irradiance_map_frame_buffer;
		
		FrameBufferGL m_blur_frame_buffer;
		

		const FrameBufferSpecifications m_shadow_map_specs;

		const static unsigned int MAX_NUMBER_OF_DIR_LIGHTS = 4, MAX_NUMBER_OF_POINT_LIGHTS = 4;
		std::array<FrameBufferGL, MAX_NUMBER_OF_DIR_LIGHTS> m_dir_light_shadow_map_textures{};

		std::vector<DirLight> m_dir_lights;
		std::vector<PointLight> m_point_lights;
		
		Material m_pbr_render_material;
		Material m_hdr_to_cubemap_render_material;
		Material m_direct_shadow_map_material;
		Material m_skybox_material;
		Material m_irradiance_material;
		Material m_blur_material;
		Material m_postprocessing_material;

		Camera m_camera;

		bool m_using_environment_map;
		bool m_using_bloom;
	};
	
}



