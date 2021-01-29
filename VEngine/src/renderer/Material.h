#pragma once

#include "ShaderProgram.h"

namespace vengine
{
	class Material
	{
	public:
		explicit Material(const ShaderProgram& shader_program);

		template<class T>
		void set(const std::string& uniform_name, T value);

		void use() const;
	private:
		ShaderProgram m_shader_program;
	};
}


