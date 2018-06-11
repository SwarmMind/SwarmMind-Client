#include <menu/ConnectedState.h>
#include <functional>
#include <imgui/imgui.h>
#include <game/Game.h>
#include <renderer/Sprites.h>
#include <events/EventSystem.h>

ConnectedState::ConnectedState(Game& _game, Sprites& _sprites, Input& _input, EventSystem& _eventSystem, std::string address, unsigned port)
	: input{ _input }
	, map{nullptr}
	, game{_game}
	, sprites{_sprites}
	, networker{_eventSystem}
	, eventSystem{_eventSystem}
{
	enableCallbacks();
	networker.connect(address, port);
}

ConnectedState::~ConnectedState()
{
	delete map;
}

void ConnectedState::update(double deltaTime)
{
	networker.update();
	if (map != nullptr)
	{
		map->update(deltaTime);
	}
}

std::string ConnectedState::statusString() const {
	if (!networker.isConnected()) {
		return "Connecting...";
	}
	else {
		return "Connected!";
	}
}

void ConnectedState::drawStatus() {
	const std::string statusMessage{ statusString() };

	ImGui::SetNextWindowBgAlpha(0.5);

	ImGui::SetNextWindowPos(ImVec2(30, 30), 0);
	ImGui::Begin(statusMessage.data(), nullptr
		, ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_AlwaysAutoResize);
	{
		if (ImGui::Button("Exit"))
		{
			game.openMainMenu();
		}
		
		ImGui::SameLine();
		ImGui::Text("%s", statusMessage.data());
	}
	ImGui::End();
}

void ConnectedState::draw(Renderer& renderer)
{
	if (map != nullptr)
	{
		map->draw(renderer);
	}
	drawStatus();
}

using namespace std::placeholders;

void ConnectedState::enableCallbacks()
{
	networker.setConnectCallback(std::bind(&ConnectedState::onConnect, this));
	networker.setDisconnectCallback(std::bind(&ConnectedState::onDisconnect, this));
	networker.setInitStateCallback(std::bind(&ConnectedState::onInitState, this, _1, _2));
	networker.setStateCallback(std::bind(&ConnectedState::onState, this, _1));
	//networker.setGameOverCallback(std::bind(&ConnectedState::onDisconnect, this));
}

void ConnectedState::onInitState(Configuration config, Gamestate *gamestate) {
	map = new Map{ input, sprites, networker, eventSystem, config };
	map->updateGameState(gamestate);
}

void ConnectedState::onState(Gamestate *state)
{
	map->updateGameState(state);
}

void ConnectedState::onConnect()
{
}

void ConnectedState::onDisconnect()
{
	delete map;
	map = nullptr;
}
