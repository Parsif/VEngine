#include "precheader.h"
#include "Grid.h"

namespace vengine
{
	void Grid::init()
	{
		float vertices[12] = 
		{
			 1,  1, 0,
			-1,  1, 0,
			 1, -1, 0,
			-1, -1, 0
		};
		
		unsigned int indices[6] =
		{
			0, 1, 2, 3, 1, 2
		};

		m_render_command.set_vertex_buffer(vertices, sizeof(vertices));
		m_render_command.set_index_buffer(indices, sizeof(indices), sizeof(indices) / sizeof(unsigned int));
		m_render_command.set_buffer_layout(BufferLayout{ ShaderDataType::Float3 });
	}
}
