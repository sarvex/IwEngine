﻿#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/log/logger.h"

#include "OpenGL/imgui_impl_opengl3.h"
#include "Windows/imgui_impl_win32.h"
#include "iw/engine/Time.h"

#include "iw/math/iwmath.h"

namespace iw {
namespace Engine {
	ImGuiLayer::ImGuiLayer(
		IWindow* window,
		FontMap* fonts
	)
		: Layer     ("ImGui")
		, m_window  (window)
		, m_fonts   (fonts)
		, m_context (nullptr)
	{
		//viewportRT = REF<RenderTarget>();
		//viewportRT->AddTexture(REF<Texture>(1280, 720));
		//viewportRT->AddTexture(REF<Texture>(1280, 720, TEX_2D, DEPTH));
	}

	ImGuiLayer::~ImGuiLayer() {}

	int ImGuiLayer::Initialize() {
		IMGUI_CHECKVERSION();

		m_context = ImGui::CreateContext();
		//Bind();

		ImGui::StyleColorsDark();

		auto& io = ImGui::GetIO();
		//io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
		//if(m_window) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// loading fonts is done before init because everything turns black if I don't
		// do this here and I dont care to debug further...

		m_fonts->Fonts.emplace("default", io.Fonts->AddFontDefault());

		for (auto& [name, file, size] : m_fonts->ToLoad)
		{
			std::stringstream ss;
			ss << name;
			ss << "_";
			ss << size;

			ImFont* font = io.Fonts->AddFontFromFileTTF(Asset->GetPath(file).c_str(), (float)size);
			m_fonts->Fonts.emplace(ss.str(), font);
		}

		m_fonts->ToLoad.clear();

		io.UserData = m_fonts;

		if (m_window) ImGui_ImplWin32_Init(m_window->Handle(), m_window->Context());
		ImGui_ImplOpenGL3_Init("#version 450");

		//Renderer->SetDefaultTarget(target);

		io.KeyMap[ImGuiKey_Tab        ] = (int)InputName::TAB;
		io.KeyMap[ImGuiKey_LeftArrow  ] = (int)InputName::LEFT;
		io.KeyMap[ImGuiKey_RightArrow ] = (int)InputName::RIGHT;
		io.KeyMap[ImGuiKey_UpArrow    ] = (int)InputName::UP;
		io.KeyMap[ImGuiKey_DownArrow  ] = (int)InputName::DOWN;
		io.KeyMap[ImGuiKey_PageUp     ] = (int)InputName::PRIOR;
		io.KeyMap[ImGuiKey_PageDown   ] = (int)InputName::NEXT;
		io.KeyMap[ImGuiKey_Home       ] = (int)InputName::HOME;
		io.KeyMap[ImGuiKey_End        ] = (int)InputName::END;
		io.KeyMap[ImGuiKey_Insert     ] = (int)InputName::INSERT;
		io.KeyMap[ImGuiKey_Delete     ] = (int)InputName::DEL;
		io.KeyMap[ImGuiKey_Backspace  ] = (int)InputName::BACK;
		io.KeyMap[ImGuiKey_Space      ] = (int)InputName::SPACE;
		io.KeyMap[ImGuiKey_Enter      ] = (int)InputName::EXECUTE;
		io.KeyMap[ImGuiKey_Escape     ] = (int)InputName::ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = (int)InputName::EXECUTE; // the same?
		io.KeyMap[ImGuiKey_A          ] = (int)InputName::A;        // for text edit CTRL+A: select all
		io.KeyMap[ImGuiKey_C          ] = (int)InputName::C;        // for text edit CTRL+C: copy
		io.KeyMap[ImGuiKey_V          ] = (int)InputName::V;        // for text edit CTRL+V: paste
		io.KeyMap[ImGuiKey_X          ] = (int)InputName::X;        // for text edit CTRL+X: cut
		io.KeyMap[ImGuiKey_Y          ] = (int)InputName::Y;        // for text edit CTRL+Y: redo
		io.KeyMap[ImGuiKey_Z          ] = (int)InputName::Z;        // for text edit CTRL+Z: undo

		return 0;
	}

	void ImGuiLayer::OnPush() {
		//Renderer->Now->SetDefaultTarget(viewportRT);
	}

	void ImGuiLayer::OnPop() {
		//Renderer->Now->SetDefaultTarget(nullptr);
	}

	void ImGuiLayer::Destroy() {
		ImGui_ImplOpenGL3_Shutdown();
		if (m_window) ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::ImGui() {
		ImGui::GetIO().DeltaTime = Time::DeltaTime();

		//if (m_window) {
		//	iw::vec2 pos = Mouse::ClientPos();

		//	ImGui::GetIO().MousePos.x = pos.x();
		//	ImGui::GetIO().MousePos.y = pos.y();
		//}
	}

	void ImGuiLayer::Begin() {
		// Frame

		ImGui_ImplOpenGL3_NewFrame();
		if (m_window) ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Dockspace
		// should put if statement for if this should be enabled
		// could also make a seperate layer
		// I really dont like the layering system rn there is no
		// way to edit the ordering only push front or back...

		ImGuiWindowFlags window_flags =
			  ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Dockspace", nullptr, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("Dockspace");

		ImGui::DockSpace(dockspace_id);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", NULL, false)) {
					Console->QueueCommand("quit");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		//// temp times

		//ImGui::Begin("Times");

		//ImGui::SliderFloat("Smooth", &smoothing, 0, 1, "%1.2f", .5);
		//PrintTimes(LOG_GET_TIMES());

		//ImGui::End();

		//// temp Viewspace

		////target->Resize(size.x, size.y);

		////ImVec2 size;
		////size.x = target->Tex(0)->Width();
		////size.y = target->Tex(0)->Height();

		//if (viewportRT->Tex(0)->Handle()) {
		//	ImVec2 size = ImGui::GetWindowSize();

		//	if (   (unsigned)size.x != viewportRT->Width()
		//		|| (unsigned)size.y != viewportRT->Height())
		//	{
		//		viewportRT->Resize((unsigned)size.x, (unsigned)size.y);
		//	}

		//	ImGui::PushClipRect(ImVec2(0,0), ImVec2(size.x, size.y), false);
		//	ImGui::Begin("Viewspace");

		//	unsigned id = viewportRT->Tex(0)->Handle()->Id();
		//	ImGui::Image((void*)(size_t)id, size, ImVec2(0, 1), ImVec2(1, 0));
		//	
		//	ImGui::End();
		//	ImGui::PopClipRect();
		//}
	}

	void ImGuiLayer::End() {
		// Dockspace

		//ImGui::End();

		// Frame

		ImGui::Render(); // calls EndFrame
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			//m_window->ReleaseOwnership();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			
			m_window->TakeOwnership();
		}
	}

	bool ImGuiLayer::On(
		WindowResizedEvent& e)
	{
		auto& io = ImGui::GetIO();

		io.DisplaySize.x = (float)e.Width;
		io.DisplaySize.y = (float)e.Height;

		return false;
	}

	bool ImGuiLayer::On(
		MouseMovedEvent& e)
	{
		auto& io = ImGui::GetIO();
		
		if (e.Device == DeviceType::MOUSE)
		{
			glm::vec2 screenPos = glm::vec2(e.X, e.Y)/*iw::Mouse::ScreenPos()*/;

			io.MousePos.x = (float)screenPos.x;
			io.MousePos.y = (float)screenPos.y;

			return io.WantCaptureMouse;
		}

		return false;
	}

	bool ImGuiLayer::On(
		MouseWheelEvent& e)
	{
		auto& io = ImGui::GetIO();
		
		if (e.Device == DeviceType::MOUSE)
		{
			if (io.WantCaptureMouse) {
				io.MouseWheel += e.Delta;
			}

			return io.WantCaptureMouse;
		}

		return false;
	}

	bool ImGuiLayer::On(
		MouseButtonEvent& e)
	{
		auto& io = ImGui::GetIO();
		
		if (   e.Device == DeviceType::MOUSE
			&& io.WantCaptureMouse)
		{
			io.MouseDown[(int)e.Button - (int)InputName::LMOUSE] = e.State;
			return io.WantCaptureMouse;
		}

		return false;
	}

	bool ImGuiLayer::On(
		KeyEvent& e)
	{
		auto& io = ImGui::GetIO();

		if (io.WantCaptureKeyboard)
		{
			io.KeysDown[(int)e.Button] = e.State;
			return io.WantCaptureKeyboard;
		}

		return false;
	}

	bool ImGuiLayer::On(
		KeyTypedEvent& e)
	{
		auto& io = ImGui::GetIO();

		if (io.WantCaptureKeyboard)
		{
			io.AddInputCharacter(e.Character);
			return io.WantCaptureKeyboard;
		}

		return false;
	}
	
	bool ImGuiLayer::On(
		OsEvent& e)
	{
		return false;// ImGui_ImplWin32_WndProcHandler(e.Handle, e.Message, e.WParam, e.LParam);
	}
}
}
