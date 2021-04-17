#include "precheader.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include "scene/Components.h"


namespace vengine
{
	void SceneHierarchyPanel::init(Ref<Scene> scene)
	{
        m_scene = scene;
	}


	void SceneHierarchyPanel::draw()
	{
		ImGui::Begin("SceneHierarchyPanel");
		
        m_scene->m_registry.each([this](auto entity)
        {
            draw_entity_node(entity);
        });
		
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItem("Create dir light"))
                m_scene->create_dir_light();

            ImGui::EndPopup();
        }

		ImGui::End();

        ImGui::Begin("Properties");
		if(m_scene->m_registry.valid(m_selected_entity))
		{
            draw_entity_components(m_selected_entity);
		}
        ImGui::End();
	}

	void SceneHierarchyPanel::draw_entity_node(entt::entity entity)
	{
        const auto& tag = m_scene->m_registry.get<TagComponent>(entity).tag;
        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
            (m_selected_entity == entity ? ImGuiTreeNodeFlags_Selected : 0);
        const bool opened = ImGui::TreeNodeEx((void*)entity, flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_selected_entity = entity;
        }
		
        if (opened)
        {
            ImGui::TreePop();
        }
	}

	void SceneHierarchyPanel::draw_entity_components(entt::entity entity) const
	{
        if (m_scene->m_registry.has<TagComponent>(entity))
        {
            auto& tag = m_scene->m_registry.get<TagComponent>(entity).tag;
            if (ImGui::InputText("Tag", tag.data(), tag.size() * sizeof(char)))
            {
            }
        }

        constexpr float DRAG_SPEED = 0.1f;

        if (m_scene->m_registry.has<TransformComponent>(entity))
        {
            auto& transform_component = m_scene->m_registry.get<TransformComponent>(entity);

            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
            {
                ImGui::DragFloat3("Position", glm::value_ptr(transform_component.translation), DRAG_SPEED);
                ImGui::DragFloat3("Rotation", glm::value_ptr(transform_component.rotation), DRAG_SPEED);
                ImGui::DragFloat3("Scale", glm::value_ptr(transform_component.scale), DRAG_SPEED);
                ImGui::TreePop();
            }
        }

		
        if (m_scene->m_registry.has<CameraComponent>(entity))
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera prop"))
            {
               
                const std::string fov = "Fov: " + std::to_string((int)m_scene->get_camera().get_fov());
                ImGui::Text(fov.c_str());

                if (ImGui::DragFloat("Near_z", m_scene->get_camera().get_near_z_pointer(), DRAG_SPEED))
                {
                    m_scene->get_camera().recalculate_projection();
                }

                if (ImGui::DragFloat("Far_z", m_scene->get_camera().get_far_z_pointer(), DRAG_SPEED))
                {
                    m_scene->get_camera().recalculate_projection();
                }

            	//TODO: fix
                if (ImGui::DragFloat3("Position", m_scene->get_camera().get_position_pointer(), DRAG_SPEED))
                {
                    m_scene->get_camera().recalculate_view();
                }

                ImGui::TreePop();
            }
        }

        if (m_scene->m_registry.has<DirLightComponent>(entity))
        {
            auto& dir_light_component = m_scene->m_registry.get<DirLightComponent>(entity);

            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "DirLight"))
            {
                ImGui::DragFloat3("Direction", glm::value_ptr(dir_light_component.direction), DRAG_SPEED);
                ImGui::DragFloat3("Color", glm::value_ptr(dir_light_component.color), DRAG_SPEED);
                ImGui::TreePop();
            }
        }


	}


}
