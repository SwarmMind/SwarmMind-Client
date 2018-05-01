#include <imgui/imgui.h>
#include <imgui/opengl3_example/imgui_impl_glfw_gl3.h>
#include <imgui/imgui_internal.h>
#include <renderer/ImGuiRenderer.h>

using namespace std;

ImGuiRenderer::ImGuiRenderer(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGui_ImplGlfwGL3_Init(window, true);
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiRenderer::preRender()
{	
	ImGui_ImplGlfwGL3_NewFrame(); 
}

void ImGuiRenderer::render()
{
	ImGui::Render(); 
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}


