#include "precheader.h"
#include "Camera.h"


#include "core/Logger.h"
#include "events/MouseEvents.h"

namespace vengine
{
	Camera::Camera(const float fov, const float near_z, const float far_z) :
		m_fov(fov), m_near_z(near_z), m_far_z(far_z)
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

		case EventType::MOUSE_MOVED:
		{
			const auto move_event = *static_cast<const MouseMoveEvent*>(&event);
			orbit(move_event.get_xoffset() * mouse_sensitivity, move_event.get_yoffset() * mouse_sensitivity);

			break;
		}
		default:
		{
			Logger::log("Unhandled camera event", Logger::MessageSeverity::WARNING);
			std::cout << "Unhandled camera event\n";
		}
		}
	}	

	void Camera::orbit(const float delta_x, const float delta_y)
	{
		glm::vec4 position{ m_eye.x, m_eye.y, m_eye.z, 1 };
		const glm::vec4 pivot{ m_target.x, m_target.y, m_target.z, 1 };

		glm::mat4 rotation_matrix_x{1.0f};
		rotation_matrix_x = glm::rotate(rotation_matrix_x, glm::radians(delta_x), m_up);
		position = (rotation_matrix_x * (position - pivot)) + pivot;

		const glm::vec3 right_vector{ m_view[0][0], m_view[0][1], m_view[0][2] };
		glm::mat4 rotation_matrix_y{ 1.0f };
		rotation_matrix_y = glm::rotate(rotation_matrix_y, glm::radians(delta_y), right_vector);
		m_eye = rotation_matrix_y * (position - pivot) + pivot;

		recalculate_view();
	}

	void Camera::set_projection(const float fov, const float aspect_ratio, const float near_z, const float far_z)
	{
		m_projection = glm::perspective(glm::radians(m_fov), m_aspect_ratio, m_near_z, m_far_z);
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

