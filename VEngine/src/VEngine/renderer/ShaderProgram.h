#pragma once

#include "core/Vtypes.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "core/Logger.h"

namespace vengine
{
	//forward declarations
	class ShaderProgram;
	class Material;

	enum class ShaderType
	{
		VERTEX, FRAGMENT, GEOMETRY, COMPUTE
	};
	
	class Shader
	{
	public:
		Shader(ShaderType shader_type, const std::string& filename);
		~Shader();

	private:
		[[nodiscard]] auto get_render_id() const { return m_render_id; }
		void delete_shader();
		
		static GLenum to_glew(ShaderType type);
	private:
		unsigned int m_render_id;
		ShaderType m_type;

		friend class ShaderProgram;
		friend class ComputeShader;
	};
	
	
	template<class T>
	class UniformInfo 
	{
	public:
		UniformInfo() = default;
		UniformInfo(const int location_, const unsigned int type_) : location(location_), type(type_){}
		
		int location = 0;
		unsigned int type = 0;
		T value{};
	};

	class ShaderProgram
	{
	public:
		ShaderProgram() = default;
		ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
		ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path, const std::string& geometry_shader_path);

		
	private:
		void set_all_uniform_locations();
		
		void set_uniform_location(const std::string& uniform_name, unsigned int type);
		
		void use() const;

		void set_all_uniforms() const;
			
		template<class T>
		void set_uniform_value(const std::string& uniform_name, T& value)
		{
			if (std::is_same<T, int>::value && m_int_uniforms.count(uniform_name) == 1)
			{
				m_int_uniforms[uniform_name].value = *reinterpret_cast<int*>(&value);
			}
			else if (std::is_same<T, bool>::value && m_bool_uniforms.count(uniform_name) == 1)
			{
				m_bool_uniforms[uniform_name].value = *reinterpret_cast<bool*>(&value);
			}
			else if (std::is_same<T, float>::value && m_float_uniforms.count(uniform_name) == 1)
			{
				m_float_uniforms[uniform_name].value = *reinterpret_cast<float*>(&value);
			}
			else if (std::is_same<T, glm::vec3>::value && m_vec3_uniforms.count(uniform_name) == 1)
			{
				m_vec3_uniforms[uniform_name].value = *reinterpret_cast<glm::vec3*>(&value);
			}
			else if (std::is_same<T, glm::vec4>::value && m_vec4_uniforms.count(uniform_name) == 1)
			{
				m_vec4_uniforms[uniform_name].value = *reinterpret_cast<glm::vec4*>(&value);
			}
			else if (std::is_same<T, glm::mat4>::value && m_mat4_uniforms.count(uniform_name) == 1)
			{
				m_mat4_uniforms[uniform_name].value = *reinterpret_cast<glm::mat4*>(&value);
			}
			
			else
			{
				LOG_ERROR("Uniform type unsupported or uniform name not found: " + uniform_name);
			}
		}


	private:
		unsigned int m_render_id;
		std::unordered_map<std::string, UniformInfo<int>> m_int_uniforms;
		std::unordered_map<std::string, UniformInfo<bool>> m_bool_uniforms;
		std::unordered_map<std::string, UniformInfo<float>> m_float_uniforms;
		std::unordered_map<std::string, UniformInfo<glm::vec3>> m_vec3_uniforms;
		std::unordered_map<std::string, UniformInfo<glm::vec4>> m_vec4_uniforms;
		std::unordered_map<std::string, UniformInfo<glm::mat4>> m_mat4_uniforms;

		friend Material;
	};


	class ComputeShader
	{
	public:
		ComputeShader() = default;
		ComputeShader(const std::string& shader_path);
		void use(uint32_t num_group_x, uint32_t num_group_y) const;
		void set_image(const std::string& uniform_name, int image_id) const;
		
	private:
		unsigned int m_render_id;
		unsigned int m_shader_id;
	};

}



