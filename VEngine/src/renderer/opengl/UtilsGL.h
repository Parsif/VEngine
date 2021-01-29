#pragma once

#include <GL/glew.h>

#include "types.h"

namespace vengine
{
	class UtilsGL
	{
	public:
		static GLenum to_gl_primitive_type(PrimitiveType primitive_type);
		static GLenum to_gl_index_type(IndexType index_type);
	
	};
}

