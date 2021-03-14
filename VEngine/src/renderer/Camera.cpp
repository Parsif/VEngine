#include "precheader.h"
#include "Camera.h"

#include "events/MouseEvents.h"

namespace vengine
{
	Camera::Camera(const float fov, const float aspect_ratio, const float near_z, const float far_z,
		const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up) :
		m_fov(fov), m_aspect_ratio(aspect_ratio), m_near_z(near_z), m_far_z(far_z),
		m_eye(eye), m_target(target), m_up(up)
	{
		m_view = glm::lookAt(m_eye, m_target, m_up);
		m_projection = glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near_z, m_far_z);
	}

	void Camera::on_event(const Event& event)
	{
		switch (event.get_type())
		{
		case EventType::MOUSE_SCROLLED:
		{
			constexpr float FOV_MIN = 1.0f, FOV_MAX = 180.0f;
			constexpr int SCROLL_MULTIPLIER = 4;
				
			const auto scroll_event = *static_cast<const MouseScrollEvent*>(&event);
			m_fov -= scroll_event.get_yoffset() * SCROLL_MULTIPLIER;
			if (m_fov < FOV_MIN)
			{
				m_fov = FOV_MIN;
			}
			else if(m_fov > FOV_MAX)
			{
				m_fov = FOV_MAX;
			}
				
			recalculate_projection();
			break;
		}
		default:
		{
			std::cout << "Unhandled camera event\n";
		}
		}
	}	

	void Camera::translate(const glm::vec3& translate)
	{
		
	}

	void Camera::rotate(const glm::vec3& rotate)
	{
			
	}

	void Camera::recalculate_view()
	{
		m_view = glm::lookAt(m_eye, m_target, m_up);
	}

	void Camera::recalculate_projection()
	{
		m_projection = glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near_z, m_far_z);
	}
}

