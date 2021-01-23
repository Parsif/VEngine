#include "UtilsGL.h"


namespace VEngine
{
	GLenum UtilsGL::to_gl_primitive_type(PrimitiveType primitive_type)
	{
		GLenum ret = GL_TRIANGLES;
		switch (primitive_type)
		{
		case PrimitiveType::POINT:
			ret = GL_POINTS;
			break;
		case PrimitiveType::LINE:
			ret = GL_LINES;
			break;
		case PrimitiveType::TRIANGLE:
			ret = GL_TRIANGLES;
			break;
		default: break;
		}
		return ret;
	}

	GLenum UtilsGL::to_gl_index_type(IndexType index_type)
	{
		GLenum ret = GL_BYTE;
		switch (index_type)
		{
		case IndexType::U_INT:
			ret = GL_UNSIGNED_INT;
			break;
		case IndexType::U_SHORT:
			ret = GL_UNSIGNED_SHORT;
			break;
		default:
			break;
		}
		return ret;
	}
}

