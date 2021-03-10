#pragma once

#include "Material.h"

namespace vengine
{
	class MaterialLibrary
	{
	public:
		static void init();
		[[nodiscard]] static auto& get_material(const std::string& name) { return s_materials[name]; }
	
	private:
		inline static std::unordered_map<std::string, Material> s_materials = std::unordered_map<std::string, Material>();
	};
}


