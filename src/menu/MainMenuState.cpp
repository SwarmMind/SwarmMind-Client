#include <menu/MainMenuState.h>

#include <imgui/imgui.h>
#include <game/Game.h>
#include <algorithm>
#include <cstring>

MainMenuState::MainMenuState(Game* _game, const std::string& preset_host, const uint16_t preset_port)
	: game{_game}
{
	strcpy(address, preset_host.c_str());
	port = preset_port;
}

MainMenuState::~MainMenuState()
{

}

void MainMenuState::update(double deltaTime, double timeStamp)
{
	
}

void MainMenuState::draw(Renderer& renderer)
{
	ImVec2 position = ImGui::GetIO().DisplaySize;
	position.x /= 2;
	position.y = 100;
	ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5, 0));

	if (ImGui::Begin("Connect to server", nullptr, 
		ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoMove))
	{
		if (ImGui::InputText("Address", address, addressBufferSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			game->connectTo(address, port);
		}

		if (ImGui::Button("Connect!"))
		{
			game->connectTo(address, port);
		}

		//For spacing only
		ImGui::Dummy(ImVec2(0, 10));

		if (ImGui::CollapsingHeader("Advanced Settings"))
		{
			if (ImGui::BeginChild("Advanced", ImVec2(0, 120), true))
			{
				if (ImGui::Button("Reset"))
				{
					port = default_port;
				}
				ImGui::SameLine();
				if (ImGui::InputInt("Port", &port, 1, 100))
				{
					port = std::min(port, maximumPort);
					port = std::max(port, minimumPort);
				}

			}
			ImGui::EndChild();
		}
	}
	ImGui::End();
}

