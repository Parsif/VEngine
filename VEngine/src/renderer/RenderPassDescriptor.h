#pragma once

#include "vmath/vec.h"

namespace VEngine
{
	struct RenderPassDescriptor
	{
		vmath::Vec4f clear_color{0.0f, 0.0f, 0.0f, 1.0f};
		bool depth_test_enabled = false;
		bool stencil_test_enabled = false;
		bool need_clear_color = false;
		bool need_clear_depth = false;
		bool need_clear_stencil = false;
	};
}
