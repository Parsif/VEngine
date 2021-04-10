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
			Logger::log("Failed to init glew", Logger::MessageSeverity::ERROR);
		}

#ifdef V_CONFIGURATION_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif
	}

	void RendererApiGL::shutdown()
	{
		
	}

	void RendererApiGL::begin_render_pass(const RenderPassDescriptor& descriptor)
	{
		FrameBufferSpecifications frame_buffer_specs;
		frame_buffer_specs.width = m_viewport.width;
		frame_buffer_specs.height = m_viewport.height;
		frame_buffer_specs.frame_buffer_type = descriptor.frame_buffer_type;
		
		m_frame_buffer.create(frame_buffer_specs);

		//TODO: consider glViewport call
		m_frame_buffer.bind();

		glDepthFunc(descriptor.depth_func);

		unsigned int mask = 0;
		if (descriptor.depth_test_enabled)
		{
			glEnable(GL_DEPTH_TEST);
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
		glClear(mask);

	}

	void RendererApiGL::end_render_pass() const
	{
		m_frame_buffer.unbind();
	}

	void RendererApiGL::set_viewport(int x, int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
		m_viewport.x = x;
		m_viewport.y = y;
		m_viewport.width = width;
		m_viewport.height = height;
	}


	void RendererApiGL::draw_elements(RenderCommand& triangle_command) const
	{
		prepare_drawing(triangle_command);
		glDrawElements(UtilsGL::to_gl_primitive_type(triangle_command.get_primitive_type()),
			triangle_command.get_index_count(), UtilsGL::to_gl_index_type(triangle_command.get_index_type()), 
			(GLvoid*)triangle_command.get_index_offset());

	}

	void RendererApiGL::prepare_drawing(RenderCommand& command) const
	{
		auto& material = command.get_material();
		const auto& textures2d = command.get_textures2d();
		
		for (size_t i = 0; i < textures2d.size(); ++i)
		{
			textures2d[i].bind(i);
			material.set("u_material." + textures2d[i].get_string_type(), (int)i);
		}

		material.use();

		const auto& vertex_array = command.get_vertex_array();
		if (vertex_array->is_data_bound())
		{
			vertex_array->bind_draw();
		}

		else
		{
			vertex_array->bind_data();
			command.get_vertex_buffer()->bind();
			command.get_index_buffer()->bind();
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

