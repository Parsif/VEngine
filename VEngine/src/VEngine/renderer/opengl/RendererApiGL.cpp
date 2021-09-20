#include "precheader.h"

#include "RendererApiGL.h"
#include "UtilsGL.h"

#include <GL/glew.h>

namespace vengine
{
	void RendererApiGL::init()
	{
		if (glewInit() != GLEW_OK)
		{
			LOG_ERROR("Failed to init glew");
		}

#ifdef V_CONFIGURATION_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void RendererApiGL::shutdown()
	{
		
	}

	void RendererApiGL::begin_render_pass(const RenderPassDescriptor& descriptor)
	{
		uint32_t mask = 0;
		//TODO: call only when changed
		if (descriptor.depth_test_enabled)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(descriptor.depth_func);
		}
		
		if (descriptor.need_clear_color)
		{
			mask |= GL_COLOR_BUFFER_BIT;
			glClearColor(descriptor.clear_color.x, descriptor.clear_color.y, descriptor.clear_color.z, descriptor.clear_color.w);
		}
		
		if(descriptor.need_clear_depth)
		{
			mask |= GL_DEPTH_BUFFER_BIT;
		}
		
		//TODO: call only when changed
		if (descriptor.need_culling)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(descriptor.culling_func);
		}
		
		glClear(mask);
	}

	void RendererApiGL::end_render_pass()
	{
	}

	void RendererApiGL::set_viewport(int x, int y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}


	void RendererApiGL::draw_elements(RenderCommand& command)
	{
		prepare_drawing(command);

		glDrawElements(UtilsGL::to_gl_primitive_type(command.get_primitive_type()),
			command.get_index_count(), UtilsGL::to_gl_index_type(command.get_index_type()),
			(GLvoid*)command.get_index_offset());
	}

	void RendererApiGL::draw_arrays(RenderCommand& command, uint32_t first, uint32_t count)
	{
		prepare_drawing(command);
		glDrawArrays(UtilsGL::to_gl_primitive_type(command.get_primitive_type()), first, count);
	}


	void RendererApiGL::dispatch_compute(uint32_t num_group_x, uint32_t num_group_y, uint32_t num_group_z)
	{
		glDispatchCompute(num_group_x, num_group_y, num_group_z);
	}

	void RendererApiGL::prepare_drawing(RenderCommand& command)
	{
		const auto& vertex_array = command.get_vertex_array();
		if (vertex_array->is_data_bound())
		{
			vertex_array->bind_draw();
		}
			
		else
		{
			vertex_array->bind_data();
			command.get_vertex_buffer()->bind();
			const auto& index_buffer = command.get_index_buffer();
			if(index_buffer)
			{
				index_buffer->bind();
			}
			const auto& buffer_layout = command.get_buffer_layout();
			size_t i = 0;
			for (auto&& attribute : buffer_layout.getElements())
			{
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, attribute.GetComponentCount(), GL_FLOAT, GL_FALSE, buffer_layout.getStride(), (const void*)attribute.offset);
				++i;
			}
		}
	}
}

