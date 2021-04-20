#pragma once

#include "core/Vtypes.h"

#include "Material.h"

#include "opengl/Buffer.h"
#include "opengl/types.h"
#include "opengl/VertexArray.h"
#include "opengl/TextureGL.h"


namespace vengine
{
	// TODO: consider making RenderCommand a struct
	class RenderCommand 
	{
	public:
		void set_vertex_buffer(void* data, unsigned int size);
		void set_vertex_buffer(const VertexBuffer& vertex_buffer);

		void set_index_buffer(unsigned int* data, unsigned int size, size_t count);
		void set_index_buffer(const IndexBuffer& index_buffer);

		void set_buffer_layout(const BufferLayout& buffer_layout);
		void set_transform(const glm::mat4& transform);

		void set_textures2d(std::vector<TextureGL> textures2d);


		[[nodiscard]] auto& get_vertex_buffer() const { return m_vertex_buffer; }
		[[nodiscard]] auto& get_index_buffer() const { return m_index_buffer; }
		[[nodiscard]] auto& get_vertex_array() const { return m_vertex_array; }
		
		[[nodiscard]] auto get_primitive_type() const { return m_primitive_type; }
		[[nodiscard]] auto get_index_count() const { return m_index_buffer->get_count(); }
		[[nodiscard]] auto get_index_type() const { return m_index_type; }
		[[nodiscard]] auto get_index_offset() const { return m_index_buffer->get_offset(); }
		[[nodiscard]] auto& get_buffer_layout() const { return m_buffer_layout; }
		[[nodiscard]] auto& get_transform() const { return m_transform; }

		[[nodiscard]] auto& get_textures2d() const { return m_textures2d; }

		
		
	private:
		Ref<VertexBuffer> m_vertex_buffer;
		Ref<IndexBuffer> m_index_buffer;
		Ref<VertexArray> m_vertex_array = std::make_shared<VertexArray>();
		BufferLayout m_buffer_layout{};
		glm::mat4 m_transform{1.0f};
		
		std::vector<TextureGL> m_textures2d;

		PrimitiveType m_primitive_type = PrimitiveType::TRIANGLE;
		IndexType m_index_type = IndexType::U_INT;
	};
}


