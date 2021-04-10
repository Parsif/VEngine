#include "precheader.h"

#include "RenderCommand.h"

namespace vengine
{
	void RenderCommand::set_vertex_buffer(void* data, unsigned int size)
	{
		m_vertex_buffer = std::make_shared<VertexBuffer>(data, size);
	}
	
	void RenderCommand::set_index_buffer(unsigned* data, unsigned int size, size_t count)
	{
		m_index_buffer = std::make_shared<IndexBuffer>(data, size, count);
	}

	void RenderCommand::set_buffer_layout(const BufferLayout& buffer_layout)
	{
		m_buffer_layout = buffer_layout;
	}

	void RenderCommand::set_material(Material material)
	{
		m_material = std::move(material);
	}

	void RenderCommand::set_textures2d(std::vector<TextureGL> textures2d)
	{
		m_textures2d = std::move(textures2d);
	}
}
