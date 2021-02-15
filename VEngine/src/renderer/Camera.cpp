#include "precheader.h"
#include "Camera.h"

namespace vengine
{
	Camera::Camera(const glm::mat4& projection_matrix, const glm::vec3& eye, const glm::vec3& up,
		const glm::vec3& target) : m_projection(projection_matrix)
	{
		m_view = glm::lookAt(eye, target, up);
	}

	void Camera::on_event(const Event& event)
	{
		std::cout << "camera event\n";
		
	}
}

