#pragma once

#include <GL/glew.h>

#include "types.h"

namespace vengine
{
	class UtilsGL
	{
	public:
		[[nodiscard]] static GLenum to_gl_primitive_type(PrimitiveType primitive_type);
		[[nodiscard]] static GLenum to_gl_index_type(IndexType index_type);
	};
}

