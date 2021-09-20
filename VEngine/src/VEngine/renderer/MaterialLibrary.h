#pragma once

#include "Material.h"

namespace vengine
{
	class MaterialLibrary
	{
	public:
		static Material& load(const std::string& compute_shader, const std::string& name);
		static Material& load(const std::string& vertex_shader, const std::string& fragment_shader, const std::string& name);
		static Material& load(const std::string& vertex_shader, const std::string& fragment_shader, const std::string& geometry_shader, const std::string& name);

		[[nodiscard]] static auto& get_material(const std::string& name) { return s_materials[name]; }
	
	private:
		inline static std::unordered_map<std::string, Material> s_materials = std::unordered_map<std::string, Material>();
	};
}


