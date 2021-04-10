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
			if (message.severity == Logger::MessageSeverity::ERROR)
			{
				ImGui::TextColored(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }, message.text.c_str());
			}
			else if(message.severity == Logger::MessageSeverity::WARNING)
			{
				ImGui::TextColored(ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f }, message.text.c_str());
			}
			else if (message.severity == Logger::MessageSeverity::INFO)
			{
				ImGui::TextColored(ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f }, message.text.c_str());
			}
		}
		ImGui::End();
	}
}

