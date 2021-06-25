#include "precheader.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>

#include "imgui_internal.h"
#include "renderer/ModelLoader.h"
#include "scene/Components.h"
#include "utils/PlatformUtils.h"


namespace vengine
{
	void SceneHierarchyPanel::init(Ref<Window> window, Ref<Scene> scene) 
	{
        m_window = window;
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
            {
                m_scene->create_dir_light();
            }

            if (ImGui::MenuItem("Create camera"))
            {
                m_scene->create_camera();
            }

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


        if (m_scene->m_registry.has<TransformComponent>(entity))
        {
            auto& transform_component = m_scene->m_registry.get<TransformComponent>(entity);

            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
            {
                if(draw_vec3_component("Position", transform_component.translation))
                {
                    if (m_scene->m_registry.has<DirLightComponent>(entity))
                    {
                        auto& dir_light_component = m_scene->m_registry.get<DirLightComponent>(entity);
                        m_scene->m_registry.replace<DirLightComponent>(entity, dir_light_component);
                    }
                }
                draw_vec3_component("Rotation", transform_component.rotation);
                draw_vec3_component("Scale", transform_component.scale);
                ImGui::TreePop();
            }
        }


        if (m_scene->m_registry.has<CameraComponent>(entity))
        {
            constexpr float DRAG_SPEED = 0.1f;
            auto& camera = m_scene->m_registry.get<CameraComponent>(entity).camera;

            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera prop"))
            {
                const std::string fov = "Fov: " + std::to_string((int)camera.get_fov());
                ImGui::Text(fov.c_str());


                ImGui::Text("Near_z");
                ImGui::SameLine();
                if (ImGui::DragFloat("##Near_z", camera.get_near_z_pointer(), DRAG_SPEED))
                {
                    m_scene->get_editor_camera().recalculate_projection();
                }

                ImGui::Text("Far_z");
                ImGui::SameLine();
                if (ImGui::DragFloat("##Far_z", camera.get_far_z_pointer(), DRAG_SPEED))
                {
                    m_scene->get_editor_camera().recalculate_projection();
                }
               
                ImGui::TreePop();
            }
        }

        if (m_scene->m_registry.has<DirLightComponent>(entity))
        {
            constexpr float DRAG_SPEED = 0.05f;

            auto& dir_light_component = m_scene->m_registry.get<DirLightComponent>(entity);

            if (ImGui::TreeNodeEx((void*)typeid(DirLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "DirLight"))
            {
                if (draw_vec3_component("Color", dir_light_component.color))
                {
                    m_scene->m_registry.replace<DirLightComponent>(entity, dir_light_component);
                }

                ImGui::Text("Intensity");
                ImGui::SameLine();
                if(ImGui::DragFloat("##Intensity", &dir_light_component.intensity, DRAG_SPEED))
                {
                    m_scene->m_registry.replace<DirLightComponent>(entity, dir_light_component);
                }

                ImGui::TreePop();
            }
        }

		if(m_scene->m_registry.has<MaterialsComponent>(entity))
		{
            if (ImGui::TreeNodeEx((void*)typeid(MaterialsComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Materials"))
            {
                MaterialsComponent& materials = m_scene->m_registry.get<MaterialsComponent>(entity);
                const std::string& filepath = m_scene->m_registry.get<ModelComponent>(entity).filepath;
                Mesh& mesh = ModelLoader::get_mesh(filepath);
            	
                if (!mesh.has_albedo_texture)
                {
                    draw_material_texture_component(materials.albedo_texture, aiTextureType_DIFFUSE, "Albedo");
                }

                if (!mesh.has_metallic_texture)
                {
                    draw_material_texture_component(materials.metallic_texture, aiTextureType_METALNESS, "Metallic");
                }

                if (!mesh.has_roughness_texture)
                {
                    draw_material_texture_component(materials.roughness_texture, aiTextureType_SHININESS, "Roughness");
                }
                if (!mesh.has_ao_texture)
                {
                    draw_material_texture_component(materials.ao_texture, aiTextureType_AMBIENT_OCCLUSION, "AO");
                }

                if (!mesh.has_normal_texture)
                {
                    draw_material_texture_component(materials.normal_texture, aiTextureType_NORMALS, "Normal");
                }
            	
                ImGui::TreePop();
            }
		}
	}

    bool SceneHierarchyPanel::draw_vec3_component(const std::string& label, glm::vec3& values, float reset_value, float column_width)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, column_width);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = reset_value;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        bool is_used = ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = reset_value;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        is_used |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Z", buttonSize))
            values.z = reset_value;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        is_used |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();

        return is_used;
    }

	void SceneHierarchyPanel::draw_material_texture_component(TextureGL& texture, aiTextureType texture_type,
		const std::string& label) const
	{
        ImGui::Text((label + " texture").c_str());
        ImGui::SameLine();
        if (ImGui::Button(("Choose file##" + label).c_str()))
        {
            const auto filename = FileDialogs::open_file_dialog("Png, Jpeg, Jpg, Tga (.png, .jpeg, .jpg, .tga)\0*.png;*.jpeg;*.jpg;*.tga\0", m_window);
            if (filename)
            {
                texture = TextureGL{ filename.value(), texture_type };
            }
        }
        ImGui::SameLine();
		if(ImGui::Button(("Delete##" + label).c_str()))
		{
            texture = TextureGL{};
		}
        if (texture)
        {
            ImGui::Image((void*)texture.get_id(), ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
        }
	}
}
