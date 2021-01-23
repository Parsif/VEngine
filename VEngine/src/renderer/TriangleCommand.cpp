#include "TriangleCommand.h"

namespace VEngine
{
	void TriangleCommand::set_vertex_buffer(void* data, unsigned int size)
	{
		m_vertex_buffer = std::make_shared<VertexBuffer>(data, size);
	}

	void TriangleCommand::set_index_buffer(unsigned* data, unsigned int size, size_t count)
	{
		m_index_buffer = std::make_shared<IndexBuffer>(data, size, count);
	}

	void TriangleCommand::set_material(Ref<Material> material)
	{
		m_material = std::move(material);
	}
}
