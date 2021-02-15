#include "precheader.h"

#include "Material.h"


namespace vengine
{
	Material::Material(const ShaderProgram& shader_program) : m_shader_program(shader_program)
	{
	}

	void Material::use() const
	{
		m_shader_program.use();
		m_shader_program.set_all_uniforms();
	}
}
