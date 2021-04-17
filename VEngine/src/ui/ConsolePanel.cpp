#include "precheader.h"
#include "ConsolePanel.h"

#include "core/Logger.h"

#include <imgui.h>


namespace vengine
{
	void ConsolePanel::draw()
	{
		ImGui::Begin("Console");
		for (auto&& message : Logger::get_messages())
		{
			ImGui::TextColored(message.severity_color, message.text.c_str());
		}
		ImGui::SetScrollHere(1.0f);
		ImGui::End();
	}
}

