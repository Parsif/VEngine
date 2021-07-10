#include "precheader.h"
#include "ImGuiUI.h"

#include "renderer/Renderer.h"
#include "events/KeyEvent.h"
#include "math/Math.h"
#include "utils/PlatformUtils.h"
#include "scene/SceneSerializer.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

#include <ImGuizmo.h>
#include <GLFW/glfw3.h>

#include "core/Logger.h"
#include "utils/Timer.h"


namespace vengine
{
	void ImGuiUI::init(Ref<Window> window, Ref<Scene> scene, Ref<Renderer> renderer)
	{
		m_window = window;
		m_scene = scene;
		m_renderer = renderer;

		m_scene_hierarchy_panel.init(window, scene);
		m_scene_settings_panel.init(window, scene);

		init_imgui();
	}

	void ImGuiUI::init_imgui() const
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImFont* pFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		io.FontDefault = io.Fonts->Fonts.back();

		set_edin_black_style();

		auto* glfw_window = static_cast<GLFWwindow*>(m_window->get_native());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}


	void ImGuiUI::draw()
	{
		begin_frame();
		render_docking_begin();

		// scene view
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 8));
		static ImVec2 scene_view_size;
		ImGui::Begin("SceneView");
		if(ImGui::Button("Play"))
		{
			m_scene->start_game();
			m_guizmo_type = -1; //TODO: change this
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			m_scene->stop_game();
		}
		
		ImVec2 size = ImGui::GetWindowSize();
		if((int)scene_view_size.x != (int)size.x || (int)scene_view_size.y != (int)size.y)
		{
			scene_view_size = size;
			m_scene->get_active_camera().set_aspect_ratio(scene_view_size.x / scene_view_size.y);
		}
		ImGui::Image((void*)m_renderer->get_color_attachment(), ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		draw_guizmo();
		m_scene_view_focused = ImGui::IsWindowFocused();
		ImGui::End();

		m_scene_hierarchy_panel.draw();
		m_console_panel.draw();
		m_scene_settings_panel.draw();
		

		ImGui::Begin("Debug");
		for (auto&& time : Timer::get_times())
		{
			ImGui::Text((time.name + ": " + std::to_string(time.duration) + "ms").c_str());
		}
		Timer::clear();
		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::End();

		end_frame();
	}

	void ImGuiUI::on_event(const Event& event)
	{
		if(event.get_type() == EventType::KEY_PRESSED)
		{
			const auto key_pressed_event = *static_cast<const KeyPressedEvent*>(&event);
			switch (key_pressed_event.get_keycode())
				{
			case GLFW_KEY_Q:
				if (!m_scene_view_focused) return; //TODO: fix this
				m_guizmo_type = -1;
				break;
				
			case GLFW_KEY_W:
				if (!m_scene_view_focused) return; //TODO: fix this
				m_guizmo_type = ImGuizmo::OPERATION::TRANSLATE;
				break;
				
			case GLFW_KEY_E:
				if (!m_scene_view_focused) return; //TODO: fix this
				m_guizmo_type = ImGuizmo::OPERATION::ROTATE;
				break;
				
			case GLFW_KEY_R:
				if (!m_scene_view_focused) return; //TODO: fix this
				m_guizmo_type = ImGuizmo::OPERATION::SCALE;
				break;

			case GLFW_KEY_DELETE:
				{
					const auto selected_entity = m_scene_hierarchy_panel.get_selected_entity();
					if(m_scene->m_registry.valid(selected_entity))
					{
						m_scene->destroy_entity(selected_entity);
					}
					break;
				}
			}
		}

		else
		{
			LOG_WARNING("Unhandled imgui event");
		}
	}

	void ImGuiUI::begin_frame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiUI::end_frame() const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(m_window->get_width(), m_window->get_height());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}

	void ImGuiUI::shutdown() const
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiUI::set_edin_black_style() const
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.ChildRounding = 3.f;
		style.GrabRounding = 0.f;
		style.WindowRounding = 0.f;
		style.ScrollbarRounding = 3.f;
		style.FrameRounding = 3.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowBorderSize = style.FrameBorderSize = 1.f;
		style.Colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.98f, 0.98f, 0.98f, 0.50f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.90f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.31f, 0.31f, 0.31f, 0.45f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.19f, 0.19f, 0.19f, 0.20f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.87f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.30f, 0.30f, 0.30f, 0.60f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 0.30f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 0.40f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 0.52f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.60f, 0.60f, 0.34f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.84f, 0.84f, 0.34f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.36f, 0.59f, 0.61f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

		style.TabBorderSize = 0;
		style.TabRounding = (int)(style.WindowRounding + style.ChildRounding) / 2;
	}

	void ImGuiUI::set_maya_style() const
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.ChildRounding = 3.f;
		style.GrabRounding = 0.f;
		style.WindowRounding = 0.f;
		style.ScrollbarRounding = 3.f;
		style.FrameRounding = 3.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowBorderSize = style.FrameBorderSize = 1.f;

		style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);

		style.TabBorderSize = 0;
		style.TabRounding = (int)(style.WindowRounding + style.ChildRounding) / 2;
	}

	void ImGuiUI::render_docking_begin() const
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->GetWorkPos());
			ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		static bool p_open = true;
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save scene", nullptr, false))
				{
					auto filename = FileDialogs::save_file_dialog("VEngine (.vengine)\0*.vengine\0", m_window);
					if (filename)
					{
						SceneSerializer::serialize(filename.value(), m_scene);
					}
				}

				if (ImGui::MenuItem("Open scene", nullptr, false))
				{
					auto filename = FileDialogs::open_file_dialog("VEngine (.vengine)\0*.vengine\0", m_window);
					if (filename)
					{
						SceneSerializer::deserialize(filename.value(), m_scene);
					}
				}
				ImGui::Separator();

				if (ImGui::MenuItem("Import model", nullptr, false))
				{
					auto filename = FileDialogs::open_file_dialog("Obj, fbx, gltf, dae (.obj, .fbx, .gltf, .dae)\0*.obj;*.fbx;*.gltf;*.dae\0", m_window);
					if(filename)
					{
						std::replace(filename.value().begin(), filename.value().end(), '\\', '/');
						m_scene->create_model(filename.value());
					}
				}
				
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Grid", nullptr, &Grid::s_enabled);
				ImGui::MenuItem("Padding", nullptr, &opt_padding);
				ImGui::Separator();
				
				static int style_idx = -1;
				if (ImGui::Combo("Style", &style_idx, "Edin black\0Maya\0Classic\0"))
				{
					switch (style_idx)
					{
					case 0: set_edin_black_style(); break;
					case 1: set_maya_style(); break;
					case 2: ImGui::StyleColorsClassic(); break;
					}
				}
				ImGui::EndMenu();

			}
		
			
			ImGui::EndMenuBar();


			
		}

	}

	void ImGuiUI::draw_guizmo() const
	{
		const auto selected_entity = m_scene_hierarchy_panel.get_selected_entity();
		if(m_guizmo_type != -1 && m_scene->m_registry.valid(selected_entity) && m_scene->m_registry.has<TransformComponent>(selected_entity))
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

			auto& transform_component = m_scene->m_registry.get<TransformComponent>(selected_entity);
			auto transform = transform_component.get_transform();
			
			const bool snap = m_window->is_key_pressed(GLFW_KEY_LEFT_ALT);
			
			float snap_value = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_guizmo_type == ImGuizmo::OPERATION::ROTATE)
				snap_value = 45.0f;

			float snap_values[3] = { snap_value, snap_value, snap_value };
			
			ImGuizmo::Manipulate(glm::value_ptr(m_scene->get_active_camera().get_view()),
				glm::value_ptr(m_scene->get_active_camera().get_projection()),
				static_cast<ImGuizmo::OPERATION>(m_guizmo_type),
				ImGuizmo::LOCAL,
				glm::value_ptr(transform),
				nullptr,
				snap ? snap_values : nullptr);
			
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				math::decompose_transform(transform, translation, rotation, scale);
				transform_component.translation = translation;
				//TODO: fix rotation
				//transform_component.rotation += rotation - transform_component.rotation;
				transform_component.scale = scale;
				if(m_scene->m_registry.has<DirLightComponent>(selected_entity))
				{
					auto& dir_light_component = m_scene->m_registry.get<DirLightComponent>(selected_entity);
					m_scene->m_registry.replace<DirLightComponent>(selected_entity, dir_light_component);
				}
			}
			
		}
	}
}
