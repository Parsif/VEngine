#pragma once

#include <GL/glew.h>
#include <glm/vec4.hpp>

namespace vengine
{
	struct RenderPassDescriptor
	{
		glm::vec4 clear_color{0.0f, 0.0f, 0.0f, 1.0f};
		bool depth_test_enabled = false;
		bool stencil_test_enabled = false;
		bool need_clear_color = false;
		bool need_clear_depth = false;
		bool need_clear_stencil = false;
		GLenum depth_func = GL_LEQUAL;	

		FrameBufferType frame_buffer_type = FrameBufferType::COLOR_DEPTH_STENCIL;
	};
}
