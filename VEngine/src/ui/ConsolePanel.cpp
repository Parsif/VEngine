#include "precheader.h"
#include "ConsolePanel.h"

#include "core/Logger.h"

#include <imgui.h>

namespace vengine
{
	void ConsolePanel::draw()
	{
        ImGui::Begin("Console");
        if (ImGui::Button("Clear"))
        {
            Logger::clear();
        }
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
        if (copy) ImGui::LogToClipboard();
        ImGui::TextUnformatted(Logger::get_imgui_text_buffer().begin());
        if (m_scroll_to_bottom)
            ImGui::SetScrollHere(1.0f);
        m_scroll_to_bottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
	}
}

