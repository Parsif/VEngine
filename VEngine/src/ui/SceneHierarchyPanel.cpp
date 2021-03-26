#include "precheader.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include "scene/Components.h"


#include <ImGuizmo.h>

namespace vengine
{
	SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene> scene) : m_scene(scene)
	{
		
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
            if (ImGui::MenuItem("Create empty entity"))
                m_scene->create_empty_entity();

            ImGui::EndPopup();
        }

		ImGui::End();

        ImGui::Begin("Properties");
        draw_entity_components(m_selected_entity);
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
            constexpr unsigned int TAG_MAX_SIZE = 100;
            tag.resize(TAG_MAX_SIZE);
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

                if (ImGui::DragFloat("Near_z", m_scene->get_camera().get_near_z(), DRAG_SPEED))
                {
                    m_scene->get_camera().recalculate_projection();
                }

                if (ImGui::DragFloat("Far_z", m_scene->get_camera().get_far_z(), DRAG_SPEED))
                {
                    m_scene->get_camera().recalculate_projection();
                }

            	//TODO: fix
                if (ImGui::DragFloat3("Position", m_scene->get_camera().get_position(), DRAG_SPEED))
                {
                    m_scene->get_camera().recalculate_view();
                }

                ImGui::TreePop();
            }
        }
	}


}
