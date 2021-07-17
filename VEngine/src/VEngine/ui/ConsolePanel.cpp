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
            m_text_buffer.clear();
            Logger::clear();
        }
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        static int type_idx = 0;
        if (ImGui::Combo("##Type", &type_idx, "All\0Info\0Warning\0Error\0"))
        {
        }
        switch (type_idx)
        {
        case 0: m_text_buffer = Logger::get_all_text_buffer(); break;
        case 1: m_text_buffer = Logger::get_info_text_buffer(); break;
        case 2: m_text_buffer = Logger::get_warning_text_buffer(); break;
        case 3: m_text_buffer = Logger::get_error_text_buffer(); break;
        }
        ImGui::Separator();
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
        if (copy) ImGui::LogToClipboard();
        ImGui::TextUnformatted(m_text_buffer.begin());
       
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
	}
}

