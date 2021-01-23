#pragma once
#include <GL/glew.h>


namespace VEngine
{

	//TODO: possible move to opengl/utils
	enum class ShaderType
	{
		VERTEX, FRAGMENT
	};

	//forward declaration
	class ShaderProgram;
	
	//TODO: uniforms location at compile time
	class Shader
	{
	public:
		Shader(ShaderType shader_type, const std::string& filename);
		~Shader();

	private:
		[[nodiscard]] auto get_render_id() const { return m_render_id; }
		void delete_shader();
		//TODO: possible move to opengl/utils
		static GLenum to_glew(ShaderType type);
	private:
		unsigned int m_render_id;
		ShaderType m_type;

		friend class ShaderProgram;
	};
}



