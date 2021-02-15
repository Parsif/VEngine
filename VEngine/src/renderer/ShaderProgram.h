#pragma once

#include "Shader.h"
#include "core/Vtypes.h"


namespace vengine
{
	class Material;
	
	class UniformInfoBase
	{
	public:
		UniformInfoBase() = default;
		virtual ~UniformInfoBase() = default;
		[[nodiscard]] virtual unsigned int get_type() const = 0;

	};

	template<class T>
	class UniformInfo final : public UniformInfoBase
	{
	public:
		UniformInfo(const int location, const unsigned int type) :
			m_location(location), m_type(type) {}
		~UniformInfo() override = default;

		[[nodiscard]] unsigned int get_type() const override { return m_type; }
		[[nodiscard]] int get_location() const { return m_location; }
		[[nodiscard]] T& get_value() { return m_value; }

	private:
		int m_location = 0;
		unsigned int m_type = 0;
		T m_value;
	};

	class ShaderProgram
	{
	public:
		ShaderProgram() = default;
		ShaderProgram(Shader& vertex_shader, Shader& fragment_shader);
		~ShaderProgram();
		
	private:
		void set_all_uniform_locations();
		
		void set_uniform_location(const std::string& uniform_name, unsigned int type);
		
		void use() const;

		void set_all_uniforms() const;
			
		//TODO: add error checking and handling
		template<class T>
		void set_uniform_value(const std::string& uniform_name, T value)
		{
			static_cast<UniformInfo<T>*>(m_uniforms_info[uniform_name])->get_value() = value;
		}
	
		[[nodiscard]] auto get_uniform_location(std::string uniform_name) const
		{
			return glGetUniformLocation(m_render_id, uniform_name.data());
		}
		

	private:
		unsigned int m_render_id;
		std::unordered_map<std::string, UniformInfoBase*> m_uniforms_info;
		
		friend Material;
	};

	

}



