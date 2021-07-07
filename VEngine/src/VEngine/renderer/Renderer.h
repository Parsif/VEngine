#pragma once

#include "Mesh.h"
#include "RenderCommand.h"

#include "RenderPassDescriptor.h"
#include "opengl/FrameBufferGL.h"
#include "opengl/SkyboxGL.h"
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
		void set_viewport_size(unsigned int width, unsigned int height);
		void add_dir_light(const DirLightComponent& dir_lights, const glm::vec3& position);
		void set_camera_params(const glm::mat4& view_projection, const glm::vec3& position);

		void set_skybox(const SkyboxGL& skybox);

		[[nodiscard]] auto get_color_attachment() const { return m_intermediate_frame_buffer.get_color_attachment(); }

		[[nodiscard]] auto get_viewport() const { return m_viewport; }
		void destroy_dir_lights();

	private:
		void begin_render_pass(const FrameBufferGL& frame_buffer);
		void end_render_pass(const FrameBufferGL& frame_buffer) const;
		void render_mesh(Mesh& mesh) ;
		void render_shadow(Mesh& mesh) const;
		void render_skybox();
		glm::mat4 calc_dir_light_space_matrix(const DirLight& dir_light) const;

	
	private:
		Viewport m_viewport;
		
		std::vector<Mesh> m_render_queue;
		RenderPassDescriptor m_render_pass_descriptor;

		FrameBufferGL m_final_frame_buffer;
		FrameBufferGL m_intermediate_frame_buffer;
		const FrameBufferSpecifications m_shadow_map_specs{ 1024, 1024, FrameBufferType::DEPTH_ONLY };

		const static unsigned int MAX_NUMBER_OF_DIR_LIGHTS = 4;
		std::array<FrameBufferGL, MAX_NUMBER_OF_DIR_LIGHTS> m_dir_light_shadow_map_textures{};

	
		
		std::vector<DirLight> m_dir_lights;
		
		Material m_pbr_render_material;
		Material m_basic_render_material;
		Material m_direct_shadow_map_material;
		Material m_skybox_material;

		SkyboxGL m_skybox;
		
		glm::mat4 m_camera_view_projection;
		glm::vec3 m_camera_pos;
	};
}



