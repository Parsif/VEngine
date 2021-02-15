#pragma once

#include "renderer/Camera.h"


namespace vengine
{
	class Scene
	{
	public:
		void on_update();
		void on_event(const Event& event);
		
		void create_command();
		
		void set_camera(const Camera& camera);
		
	private:
		Camera m_camera;

	};
}


