#pragma once
#include "core/Vtypes.h"
#include "core/Window.h"


namespace vengine
{
	enum class KeyCode
	{
		LEFT_MOUSE_BTN, RIGHT_MOUSE_BTN, MIDDLE_MOUSE_BTN,
		Q, W, E, R, T, Y, U, I, O, P,
		A, S, D, F, G, H, J, K, L,
		Z, X, C, V, B, N, M,
		LEFT_SHIFT, LEFT_ALT
	};

	class Input
	{
	public:
		static void init(Ref<Window> window);
		[[nodiscard]] static bool is_pressed(KeyCode key_code);

	
	private:
		static int to_glfw_key(KeyCode key_code);
	private:
		static inline Ref<Window> s_window = nullptr;
	};
}


