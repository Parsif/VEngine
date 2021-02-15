#pragma once

#include "events/Event.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vengine
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection_matrix, const glm::vec3& eye, const glm::vec3& up, const glm::vec3& target);
		void on_event(const Event& event);
		
		//TODO: matrix multiplication should be const
		[[nodiscard]] auto get_view_projection() { return m_projection * m_view; }
		
		
	private:
		glm::mat4 m_view;
		glm::mat4 m_projection;
		
				
	};
}


