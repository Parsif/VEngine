#include "precheader.h"
#include "SceneSettingsPanel.h"

#include "utils/PlatformUtils.h"

namespace vengine
{
	void SceneSettingsPanel::init(Ref<Window> window, Ref<Scene> scene)
	{
        m_window = window;
		m_scene = scene;
        m_scene->set_exposure(m_exposure);
	}

	void SceneSettingsPanel::draw()
	{
        ImGui::Begin("Scene settings");
        ImGui::Text("Environment map");
        ImGui::SameLine();
        if (ImGui::Button("Choose file##environment_map"))
        {
            const auto filename = FileDialogs::open_file_dialog("HDR (.hdr)\0*.hdr\0", m_window);
            if (filename)
            {
                m_environment_map_texture = TextureGL{ filename.value(), aiTextureType_NONE };
                m_scene->set_environment_texture(m_environment_map_texture);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete##environment_map"))
        {
            m_environment_map_texture = TextureGL{};
            m_scene->set_environment_texture(m_environment_map_texture);
        }
		
        if (m_environment_map_texture)
        {
            ImGui::Image((void*)m_environment_map_texture.get_id(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
        }
		
        constexpr float DRAG_SPEED = 0.1f;

        ImGui::Text("Exposure");
        ImGui::SameLine();
        if (ImGui::DragFloat("##Exposure", &m_exposure, DRAG_SPEED, 0.0f, 100.f))
        {
            m_scene->set_exposure(m_exposure);
        }

        ImGui::End();
	}
}
