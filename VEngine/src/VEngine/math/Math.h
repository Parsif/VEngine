#pragma once

#include <glm/glm.hpp>

namespace vengine::math
{
	bool decompose_transform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
}
