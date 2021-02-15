#pragma once

#include "core/Vtypes.h"

#include "Material.h"
#include "RenderCommand.h"

#include "opengl/Buffer.h"
#include "opengl/types.h"
#include "opengl/VertexArray.h"


namespace vengine
{
	class TriangleCommand final : public RenderCommand
	{
	public:
		void set_vertex_buffer(void* data, unsigned int size);
		void set_index_buffer(unsigned int* data, unsigned int size, size_t count);
		void set_material(Material material);


		[[nodiscard]] auto get_vertex_buffer() const { return m_vertex_buffer; }
		[[nodiscard]] auto get_index_buffer() const { return m_index_buffer; }
		[[nodiscard]] auto get_vertex_array() const { return m_vertex_array; }
		[[nodiscard]] auto get_material() const { return m_material; }
		
		[[nodiscard]] auto get_primitive_type() const { return m_primitive_type; }
		[[nodiscard]] auto get_index_count() const { return m_index_buffer->get_count(); }
		[[nodiscard]] auto get_index_type() const { return m_index_type; }
		[[nodiscard]] auto get_index_offset() const { return m_index_buffer->get_offset(); }

		[[nodiscard]] Type get_type() const override { return Type::TRIANGLES_COMMAND;  }
		
	private:
		Ref<VertexBuffer> m_vertex_buffer;
		Ref<IndexBuffer> m_index_buffer;
		Ref<VertexArray> m_vertex_array = std::make_shared<VertexArray>();
		Material m_material;

		PrimitiveType m_primitive_type = PrimitiveType::TRIANGLE;
		IndexType m_index_type = IndexType::U_INT;	
	};
}


