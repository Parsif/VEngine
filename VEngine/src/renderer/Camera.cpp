#include "precheader.h"
#include "Camera.h"

#include "events/Input.h"
#include "events/MouseEvents.h"
#include "events/WindowEvents.h"

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
		case EventType::WINDOW_RESIZED:
		{
			const auto window_resize_event = *static_cast<const WindowResizedEvent*>(&event);
			m_aspect_ratio = (float)window_resize_event.get_width() / window_resize_event.get_height();
			recalculate_projection();
			break;
		}
			
		case EventType::MOUSE_SCROLLED:
		{
			const auto scroll_event = *static_cast<const MouseScrollEvent*>(&event);
			const glm::vec3 direction = glm::normalize(m_eye - m_target);
			m_eye -= direction * scroll_event.get_yoffset();
		
			recalculate_view();
			break;
		}

		case EventType::MOUSE_MOVED:
		{
			const auto move_event = *static_cast<const MouseMoveEvent*>(&event);
			if(Input::is_pressed(KeyCode::MIDDLE_MOUSE_BTN))
			{
				orbit(move_event.get_xoffset() * mouse_sensitivity, move_event.get_yoffset() * mouse_sensitivity);
			}
			
			else if(Input::is_pressed(KeyCode::LEFT_SHIFT))
			{
				m_target += glm::vec3(move_event.get_xoffset() * mouse_sensitivity, 
							move_event.get_yoffset() * mouse_sensitivity, 
							move_event.get_xoffset() * mouse_sensitivity);
				recalculate_view();
			}
			
			break;
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

	void Camera::set_position(const glm::vec3& position)
	{
		m_eye = position;
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

