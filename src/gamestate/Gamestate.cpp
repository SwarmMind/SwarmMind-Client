#include <gamestate/Gamestate.h>
#include <imgui/imgui.h>
#include <vector>
#include <iostream>

using namespace std;

Gamestate::Gamestate(Networker& _networker)
	: networker{_networker}
	, gameState{nullptr}
	, textures{}
	, sprites{textures}
{

}

Gamestate::~Gamestate()
{
	delete this->gameState;
}

void Gamestate::update(double deltaTime)
{
	if (stateConfiguration.empty())
	{
		this->updateConfiguration();
	}
	else
	{
		this->updateGameState();
	}
}

void Gamestate::updateConfiguration()
{
	if (networker.isConnected())
	{
		stateConfiguration = networker.getConfiguration();
		if (!stateConfiguration.empty())
		{
			this->updateGameState();
		}
	}
}

void Gamestate::updateGameState()
{
	if (networker.isConnected())
	{
		nlohmann::json* newState = networker.getCurrentState();
		if (newState)
		{
			delete this->gameState;
			this->gameState = newState;
		}
	}
}

void Gamestate::drawStatus(std::string statusMessage)
{
	ImGui::SetNextWindowPos(ImVec2(30, 30), 0);
	ImGui::Begin(statusMessage.data(), nullptr
		, ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoInputs);
	{
		ImGui::Text(statusMessage.data());
	}
	ImGui::End();
}

void Gamestate::drawState(Renderer& renderer)
{
	int width = this->stateConfiguration["sizeX"];
	int height = this->stateConfiguration["sizeY"];

	for (size_t x = 0; x < width; x++)
	{
		for (size_t y = 0; y < height; y++)
		{
			renderer.drawSprite(x, y, 0, 1, 1, sprites.get(GridBlock));
		}
	}

	vector<nlohmann::json> mapObjects = this->gameState->at("mapObjects");
	for (nlohmann::json& mapObject : mapObjects)
	{
		int x = mapObject["posX"];
		int y = mapObject["posY"];
		string ID = mapObject["ID"];
		if (ID[0] == 'n')
			renderer.drawSprite(x, y, 1, 1, 1, sprites.get(Monster));
		else
			renderer.drawSprite(x, y, 1, 1, 1, sprites.get(Unit));
	}

}

void Gamestate::draw(Renderer& renderer)
{
	if (!networker.isConnected())
	{
		this->drawStatus("Connecting...");
	}
	else
	{
		if (this->stateConfiguration.empty() || this->gameState == nullptr)
		{
			this->drawStatus("Connected!\nWaiting for Server...");
		}
		else
		{
			this->drawStatus("Connected!");
			this->drawState(renderer);
		}
	}
}
