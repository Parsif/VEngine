#include "precheader.h"
#include "MaterialLibrary.h"

namespace vengine
{
	Material& MaterialLibrary::load(const std::string& vertex_shader, const std::string& fragment_shader,
		const std::string& name)
	{
		s_materials[name] = Material{ ShaderProgram{vertex_shader, fragment_shader} };
		return s_materials[name];
	}

	Material& MaterialLibrary::load(const std::string& vertex_shader, const std::string& fragment_shader,
		const std::string& geometry_shader, const std::string& name)
	{
		s_materials[name] = Material{ ShaderProgram{vertex_shader, fragment_shader, geometry_shader} };
		return s_materials[name];
	}
}
