#pragma once

#include "ShaderProgram.h"

namespace VEngine
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


