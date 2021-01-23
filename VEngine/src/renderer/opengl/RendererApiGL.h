#pragma once

#include "Vtypes.h"

#include "Buffer.h"
#include "types.h"
#include "VertexArray.h"

#include "../RenderPassDescriptor.h"
#include "renderer/Material.h"

namespace VEngine
{

	class RendererApiGL
	{
	public:
		RendererApiGL();
		void begin_render_pass(const RenderPassDescriptor& descriptor);
		void end_render_pass();

		void set_viewport(int x, int y, unsigned int width, unsigned int height);
		void set_vertex_buffer(Ref<VertexBuffer> vertex_buffer);
		void set_index_buffer(Ref<IndexBuffer> index_buffer);
		void set_vertex_array(Ref<VertexArray> vertex_array);
		void set_material(Ref<Material> material);


		void draw_arrays(PrimitiveType primitive_type, std::size_t start, std::size_t count) const;
		void draw_elements(PrimitiveType primitive_type, std::size_t count, IndexType index_type, std::size_t offset) const;

	private:
		void prepare_drawing() const;
		void bind_vertex_buffer() const;

#ifdef V_CONFIGURATION_DEBUG
		static void GLAPIENTRY
			MessageCallback(GLenum source,
				GLenum type,
				GLuint id,
				GLenum severity,
				GLsizei length,
				const GLchar* message,
				const void* userParam)
		{
			fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
				(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
				type, severity, message);
		}
#endif

		
	private:
		struct Viewport
		{
			int x = 0;
			int y = 0;
			unsigned int width = 0;
			unsigned int height = 0;
		};
		Ref<VertexBuffer> m_vertex_buffer;
		Ref<IndexBuffer> m_index_buffer;
		Ref<VertexArray> m_vertex_array;
		//TODO: CHANGE DEFAULT INIT OF BUFFER LAYOUT
		BufferLayout m_buffer_layout{ {ShaderDataType::Float3} };
		
		Ref<Material> m_material;
		

		Viewport m_viewport;

		
	};



	
}



