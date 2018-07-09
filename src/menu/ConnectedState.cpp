#include <menu/ConnectedState.h>
#include <functional>
#include <imgui/imgui.h>
#include <game/Game.h>
#include <renderer/Sprites.h>
#include <events/EventSystem.h>

ConnectedState::ConnectedState(Game& _game, Renderer& renderer, Input& _input, EventSystem& _eventSystem, std::string address, unsigned port)
	: input{ _input }
	, map{nullptr}
	, game{_game}
    , m_renderer{renderer}
	, networker{_eventSystem}
	, eventSystem{_eventSystem}
    , EventListener<InitStateEvent>(_eventSystem)
    , EventListener<DisconnectEvent>(_eventSystem)

{
	networker.connect(address, port);
}

ConnectedState::~ConnectedState()
{
    m_renderer.clearStaticData();
	delete map;
}

void ConnectedState::update(double deltaTime, double timeStamp)
{
	networker.update();
	if (map != nullptr)
	{
		map->update(deltaTime, timeStamp);
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

void ConnectedState::receiveEvent(InitStateEvent* event) {
    map = new Map{ input, networker, eventSystem, event->m_config };
    m_renderer.clearStaticData();
    map->drawGridStatic(m_renderer);
    map->updateGameState(event->m_state);
}

void ConnectedState::receiveEvent(DisconnectEvent* event) {
    delete map;
    map = nullptr;
    m_renderer.clearStaticData();
}
