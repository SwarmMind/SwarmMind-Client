#include <menu/MainMenuState.h>

#include <imgui/imgui.h>
#include <game/Game.h>

MainMenuState::MainMenuState(class Game* _game)
	: game{_game}
{

}

MainMenuState::~MainMenuState()
{

}

void MainMenuState::update(double deltaTime)
{
	
}

void MainMenuState::draw(Renderer& renderer)
{
	static char addressInput[256] = "127.0.0.1";
	if (ImGui::Begin("IPInput", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("Please provide an address to connect to:");
		if (ImGui::InputText("##Address", addressInput, 256, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			game->connectTo(addressInput);
		}
		if (ImGui::Button("Connect!"))
		{
			game->connectTo(addressInput);
		}
	}
	ImGui::End();
}

