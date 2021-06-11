#pragma once
#include "imgui.h"

namespace vengine
{
	class ConsolePanel
	{
	public:
		void draw();
	private:
		ImGuiTextBuffer m_text_buffer;
		bool m_scroll_to_bottom;
	};
}

