#include "precheader.h"

#include "RendererApiGL.h"
#include "UtilsGL.h"

#include <GL/glew.h>


namespace vengine
{
	RendererApiGL::RendererApiGL()
	{
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "Failed to init glew\n";
		}

#ifdef V_CONFIGURATION_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif
	
	}

	void RendererApiGL::begin_render_pass(const RenderPassDescriptor& descriptor)
	{
		FrameBufferSpecifications frame_buffer_specs;
		frame_buffer_specs.width = m_viewport.width;
		frame_buffer_specs.height = m_viewport.height;
		frame_buffer_specs.use_color_attachment = true;
		frame_buffer_specs.use_depth_stencil_attachment = true;
		m_frame_buffer.create(frame_buffer_specs);

		m_frame_buffer.bind();
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

	void RendererApiGL::end_render_pass()
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

	void RendererApiGL::set_vertex_buffer(Ref<VertexBuffer> vertex_buffer)
	{
		m_vertex_buffer = std::move(vertex_buffer);
	}

	void RendererApiGL::set_index_buffer(Ref<IndexBuffer> index_buffer)
	{
		m_index_buffer = std::move(index_buffer);
	}

	void RendererApiGL::set_vertex_array(Ref<VertexArray> vertex_array)
	{
		m_vertex_array = std::move(vertex_array);
	}

	void RendererApiGL::set_material(Ref<Material> material)
	{
		m_material = std::move(material);
	}


	void RendererApiGL::draw_arrays(PrimitiveType primitive_type, std::size_t start, std::size_t count) const
	{
		prepare_drawing();
		glDrawArrays(UtilsGL::to_gl_primitive_type(primitive_type), start, count);
	}

	void RendererApiGL::draw_elements(PrimitiveType primitive_type, std::size_t count, IndexType index_type,
		std::size_t offset) const
	{
		prepare_drawing();	
		glDrawElements(UtilsGL::to_gl_primitive_type(primitive_type), count, UtilsGL::to_gl_index_type(index_type), (GLvoid*)offset);
	}

	void RendererApiGL::prepare_drawing() const
	{
		m_material->use();
		bind_vertex_buffer();
	}

	void RendererApiGL::bind_vertex_buffer() const
	{
		if(m_vertex_array->is_data_bound())
		{
			m_vertex_array->bind_draw();
		}
		
		else
		{
			m_vertex_array->bind_data();
			m_vertex_buffer->bind();
			m_index_buffer->bind();
			size_t i = 0;
			for (auto&& attribute : m_buffer_layout.getElements())
			{
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, attribute.GetComponentCount(), GL_FLOAT, GL_FALSE, m_buffer_layout.getStride(), (const void*)attribute.offset);
				++i;
			}
		}
	
	}
}

