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
			"./VEngine/src/renderer/shaders/basic.vert" ,
			"./VEngine/src/renderer/shaders/basic.frag"
			}
		};
	}
}
