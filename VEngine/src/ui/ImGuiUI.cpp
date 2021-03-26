#include "precheader.h"
#include "ImGuiUI.h"

#include "renderer/Renderer.h"
#include "events/KeyEvent.h"
#include "math/Math.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui.h>

#include <ImGuizmo.h>
#include <GLFW/glfw3.h>



namespace vengine
{
	void ImGuiUI::init(Ref<Window> window, Ref<Scene> scene)
	{
		m_window = window;
		m_scene = scene;
		m_scene_hierarchy_panel = std::make_unique<SceneHierarchyPanel>(scene);
		init_imgui();
	}

	void ImGuiUI::init_imgui() const
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImFont* pFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		io.FontDefault = io.Fonts->Fonts.back();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

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
		
		ImGui::Begin("SceneView");
		const auto texture_id = Renderer::get_instance()->get_current_fbo().get_color_attachment_id();
		ImGui::Image((void*)texture_id, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		draw_guizmo();
		ImGui::End();

		m_scene_hierarchy_panel->draw();
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
				m_guizmo_type = -1;
				break;
				
			case GLFW_KEY_W:
				m_guizmo_type = ImGuizmo::OPERATION::TRANSLATE;
				break;
				
			case GLFW_KEY_E:
				m_guizmo_type = ImGuizmo::OPERATION::ROTATE;
				break;
				
			case GLFW_KEY_R:
				m_guizmo_type = ImGuizmo::OPERATION::SCALE;
				break;
			}
		}
		
		else
		{
			std::cerr << "Unhandled imgui event\n";
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

	ImGuiUI::~ImGuiUI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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
			if (ImGui::BeginMenu("Options"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				ImGui::MenuItem("Fullscreen", nullptr, &opt_fullscreen);
				ImGui::MenuItem("Padding", nullptr, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Close", nullptr, false, &p_open != nullptr))
					p_open = false;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

	}

	void ImGuiUI::draw_guizmo() const
	{
		const auto selected_entity = m_scene_hierarchy_panel->get_selected_entity();
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
			
			ImGuizmo::Manipulate(glm::value_ptr(m_scene->get_camera().get_view()),
				glm::value_ptr(m_scene->get_camera().get_projection()),
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
			}
			
		}
	}
}
