#include "precheader.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace vengine
{
	void Input::init(Ref<Window> window)
	{
		s_window = window;
	}

	bool Input::is_pressed(KeyCode key_code)
	{
#ifdef V_PLATFORM_WINDOWS
		if(key_code == KeyCode::LEFT_MOUSE_BTN || key_code == KeyCode::MIDDLE_MOUSE_BTN || key_code == KeyCode::RIGHT_MOUSE_BTN)
		{
			return glfwGetMouseButton(static_cast<GLFWwindow*>(s_window->get_native()), to_glfw_key(key_code)) == GLFW_PRESS;
		}
		return glfwGetKey(static_cast<GLFWwindow*>(s_window->get_native()), to_glfw_key(key_code)) == GLFW_PRESS;
#endif
	}



	int Input::to_glfw_key(KeyCode key_code)
	{
		switch (key_code)
		{
		case KeyCode::LEFT_MOUSE_BTN: return GLFW_MOUSE_BUTTON_LEFT;
		case KeyCode::RIGHT_MOUSE_BTN: return GLFW_MOUSE_BUTTON_RIGHT;
		case KeyCode::MIDDLE_MOUSE_BTN: return GLFW_MOUSE_BUTTON_MIDDLE;
		case KeyCode::Q: return GLFW_KEY_Q;
		case KeyCode::W: return GLFW_KEY_W;
		case KeyCode::E: return GLFW_KEY_E;
		case KeyCode::R: return GLFW_KEY_R;
		case KeyCode::T: return GLFW_KEY_T;
		case KeyCode::Y: return GLFW_KEY_Y;
		case KeyCode::U: return GLFW_KEY_U;
		case KeyCode::I: return GLFW_KEY_I;
		case KeyCode::O: return GLFW_KEY_O;
		case KeyCode::P: return GLFW_KEY_P;
		case KeyCode::A: return GLFW_KEY_A;
		case KeyCode::S: return GLFW_KEY_S;
		case KeyCode::D: return GLFW_KEY_D;
		case KeyCode::F: return GLFW_KEY_F;
		case KeyCode::G: return GLFW_KEY_G;
		case KeyCode::H: return GLFW_KEY_H;
		case KeyCode::J: return GLFW_KEY_J;
		case KeyCode::K: return GLFW_KEY_K;
		case KeyCode::L: return GLFW_KEY_L;
		case KeyCode::Z: return GLFW_KEY_Z;
		case KeyCode::X: return GLFW_KEY_X;
		case KeyCode::C: return GLFW_KEY_C;
		case KeyCode::V: return GLFW_KEY_V;
		case KeyCode::B: return GLFW_KEY_B;
		case KeyCode::N: return GLFW_KEY_N;
		case KeyCode::M: return GLFW_KEY_M;
		case KeyCode::LEFT_SHIFT: return GLFW_KEY_LEFT_SHIFT;
		case KeyCode::LEFT_ALT: return GLFW_KEY_LEFT_ALT;
		}
		return 0;
	}
}
