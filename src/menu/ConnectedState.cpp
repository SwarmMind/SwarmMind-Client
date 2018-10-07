#include <functional>

#include <imgui/imgui.h>
#include <game/Game.h>
#include <renderer/Sprites.h>
#include <events/EventSystem.h>
#include <events/SoundEvent.h>
#include <sound/Sounds.h>

#include <menu/ConnectedState.h>

ConnectedState::ConnectedState(Game& _game, Renderer& renderer, Input& _input, EventSystem& _eventSystem, Settings& _settings)
	: input{ _input }
	, game{_game}
    , m_renderer{renderer}
	, networker{_eventSystem}
	, eventSystem{_eventSystem}
    , EventListener<InitStateEvent>(_eventSystem)
    , EventListener<DisconnectEvent>(_eventSystem)
	, settings{ _settings }
{
	networker.connect(settings.hostname, settings.port);
}

ConnectedState::~ConnectedState()
{
	if (map) {
		settings.username = map->username();
		settings.save();
	}
    m_renderer.clearStaticData();
}

void ConnectedState::update(double deltaTime, double timeStamp)
{
	networker.update(deltaTime, timeStamp);
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
			if (map) {
				settings.username = map->username();
				settings.save();
			}
			game.openMainMenu();
		}
		
		ImGui::SameLine();
		ImGui::Text("%s", statusMessage.data());
	}
	ImGui::End();
}

void ConnectedState::draw(Renderer& renderer)
{
    if (map) {
        map->draw(renderer);
	}
	drawStatus();
}

using namespace std::placeholders;

void ConnectedState::receiveEvent(InitStateEvent* event) {
    map = std::make_unique<Map>(input, networker, eventSystem, event->m_config, settings.username);
    m_renderer.clearStaticData();
    map->drawGridStatic(m_renderer);
    map->drawWallsStatic(m_renderer, event->m_config.m_blockadePositions);
    map->updateGameState(event->m_state);
    map->m_lastUpdate -= event->m_timeSinceLastRound;

	eventSystem.processEvent(std::make_shared<SoundEvent>(SoundEnum::NextRound));
}

void ConnectedState::receiveEvent(DisconnectEvent* event) {
    map.release();
    m_renderer.clearStaticData();
}
