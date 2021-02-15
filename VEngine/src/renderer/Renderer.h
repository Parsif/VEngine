#pragma once

#include "RenderCommand.h"
#include "RenderQueue.h"

#include "opengl/RendererApiGL.h"

#include "RenderPassDescriptor.h"
#include "core/Vtypes.h"
#include "opengl/FrameBufferGL.h"

namespace vengine
{
	class Renderer
	{
	public:
		void init();
		[[nodiscard]] static Renderer* get_instance() { return s_instance; }

		void add_command(const Ref<RenderCommand> render_command);
		void render();
		void set_viewport(int x, int y, unsigned int width, unsigned int height);

		[[nodiscard]] auto& get_current_fbo() const { return m_current_frame_buffer; }
	
	private:
		void begin_render_pass();
		void end_render_pass();
		void process_render_command(const Ref<RenderCommand> command);
		void set_depth_test(bool value);
		void set_stencil_test(bool value);
		
	
	private:
		struct Viewport	
		{
			int x = 0;
			int y = 0;
			unsigned int width = 0;
			unsigned int height = 0;
		};

		Viewport m_viewport;

		
		RenderQueue m_render_queue;
		RendererApiGL m_renderer_api;

		RenderPassDescriptor m_render_pass_descriptor;
		glm::vec4 m_clear_color{0, 0, 0, 1.0f};

		FrameBufferGL m_current_frame_buffer;
		
		static Renderer* s_instance;
		
		
	};
}



