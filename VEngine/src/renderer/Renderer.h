#pragma once

#include "RenderCommand.h"
#include "RenderQueue.h"

#include "opengl/RendererApiGL.h"

#include "RenderPassDescriptor.h"
#include "opengl/FrameBufferGL.h"

namespace vengine
{
	
	
	
	class Renderer
	{
	public:
		void init();
		void shutdown();
		[[nodiscard]] static Renderer* get_instance() { return s_instance; }

		void add_drawable(const Drawable& drawable);
		
		void render();
		void set_viewport(int x, int y, unsigned int width, unsigned int height);
		void set_viewport_size(unsigned int width, unsigned int height);


		[[nodiscard]] auto get_color_attachment() const { return m_color_buffer_attachment; }

		[[nodiscard]] auto get_viewport() const { return m_viewport; }

	
	private:
		void begin_render_pass();
		void end_render_pass();
		void render_drawable(Drawable& drawable);
		void render_shadow(Drawable& drawable);

	
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

		unsigned int m_color_buffer_attachment, m_depth_buffer_attachment;
		
		static Renderer* s_instance;
	};
}



