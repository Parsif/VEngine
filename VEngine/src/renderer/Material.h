#pragma once

#include "ShaderProgram.h"

namespace vengine
{
	class Material
	{
	public:
		Material() = default;
		Material(const ShaderProgram& shader_program);


		template<class T>
		void set(const std::string& uniform_name, T value)
		{
			m_shader_program.set_uniform_value(uniform_name, value);
		}

		void use() const;
	private:
		ShaderProgram m_shader_program;
	};
}


