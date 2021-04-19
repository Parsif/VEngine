#include "precheader.h"
#include "MaterialLibrary.h"

namespace vengine
{
	void MaterialLibrary::init()
	{
		s_materials["Basic"] = Material
		{
			ShaderProgram
			{
			"./VEngine/src/renderer/shaders/basic.vert",
			"./VEngine/src/renderer/shaders/basic.frag"
			}
		};
		
		s_materials["Skybox"] = Material
		{
			ShaderProgram
			{
			"./VEngine/src/renderer/shaders/skybox.vert",
			"./VEngine/src/renderer/shaders/skybox.frag"
			}
		};

		s_materials["Grid"] = Material
		{
			ShaderProgram
			{
			"./VEngine/src/renderer/shaders/grid.vert",
			"./VEngine/src/renderer/shaders/grid.frag"
			}
		};

		s_materials["Shadowmap"] = Material
		{
			ShaderProgram
			{
			"./VEngine/src/renderer/shaders/shadowmap.vert",
			"./VEngine/src/renderer/shaders/shadowmap.frag"
			}
		};
	}
}
