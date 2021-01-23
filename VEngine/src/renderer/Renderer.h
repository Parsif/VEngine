#pragma once

#include "RenderCommand.h"
#include "RenderQueue.h"

#include "opengl/RendererApiGL.h"

#include "RenderPassDescriptor.h"

namespace VEngine
{
	class Renderer
	{
	public:
		Renderer();
		void add_command(const Ref<RenderCommand> render_command);
		void render();
		void set_viewport(int x, int y, unsigned int width, unsigned int height);

		
	private:
			
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
		vmath::Vec4f m_clear_color{0, 0, 0, 1.0f};
	};
}



