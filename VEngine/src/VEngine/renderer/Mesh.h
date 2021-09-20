#pragma once

#include "RenderCommand.h"
#include "scene/Components.h"

#include <vector>

namespace vengine
{
	struct Mesh
	{
		std::vector<RenderCommand> commands;
		bool is_casting_shadow;
		glm::mat4 transform;
		glm::mat4 prev_frame_transform;
		bool has_albedo_texture;
		bool has_roughness_texture;
		bool has_metallic_texture;
		bool has_ao_texture;
		bool has_normal_texture;
		MaterialsComponent materials;
	};
}
