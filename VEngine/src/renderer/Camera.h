#pragma once

#include "events/Event.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define M_PI 3.14159f

namespace vengine
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const float fov, const float aspect_ratio, const float near_z, const float far_z, const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);
		void on_event(const Event& event);

	
		void orbit(float delta_x, float delta_y);

		void recalculate_view();
		void recalculate_projection();
		
		[[nodiscard]] auto get_view_projection() const{ return m_projection * m_view; }
		[[nodiscard]] auto get_view() const { return m_view; }
		[[nodiscard]] auto get_projection() const { return m_projection; }

		[[nodiscard]] auto get_fov() const { return m_fov; }
		[[nodiscard]] auto* get_near_z() { return &m_near_z; }
		[[nodiscard]] auto* get_far_z() { return &m_far_z; }
		[[nodiscard]] auto get_position() { return glm::value_ptr(m_eye); }


	private:
		glm::mat4 m_view{};
		glm::mat4 m_projection{};
		float m_fov{}; // in degrees
		float m_aspect_ratio{};
		float m_near_z{}, m_far_z{};
		glm::vec3 m_eye, m_target, m_up;

		static constexpr float mouse_sensitivity = 0.3f;
	};
}


