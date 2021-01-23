#pragma once

#include "Shader.h"


namespace VEngine
{

	class ShaderProgram
	{
	public:
		ShaderProgram(Shader& vertex_shader, Shader& fragment_shader);

		void use() const;
		
	private:
		unsigned int m_render_id;
	};
}



