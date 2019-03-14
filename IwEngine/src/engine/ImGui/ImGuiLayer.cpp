#include "iw/engine/ImGui/ImGuiLayer.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "OpenGL/imgui_impl_opengl3.h"

namespace IwEngine {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui")
	{}

	ImGuiLayer::~ImGuiLayer() {}

	int ImGuiLayer::Initilize() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init("#version 410");

		return 0;
	}

	void ImGuiLayer::Destroy() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Update() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		MouseMovedEvent& event)
	{
		auto& io = ImGui::GetIO();
		io.MousePos.x = (float)event.X;
		io.MousePos.y = (float)event.Y;

		return false;
	}

	bool ImGuiLayer::On(
		MouseButtonPressedEvent& event)
	{
		auto& io = ImGui::GetIO();
		io.MouseDown[event.Button] = true;

		return false;
	}

	bool ImGuiLayer::On(
		MouseButtonReleasedEvent& event)
	{
		auto& io = ImGui::GetIO();
		io.MouseDown[event.Button] = false;

		return false;
	}
}
