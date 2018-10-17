#include <functional>

#include <imgui/imgui.h>
#include <game/Game.h>
#include <renderer/Sprites.h>
#include <events/EventSystem.h>
#include <events/SoundEvent.h>
#include <sound/Sounds.h>

#include <menu/ConnectedState.h>

ConnectedState::ConnectedState(Game& game, Renderer& renderer, Input& input, EventSystem& eventSystem, Settings& settings)
	: m_input{ input }
	, m_game{game}
    , m_renderer{renderer}
	, m_networker{eventSystem}
	, m_eventSystem{eventSystem}
    , EventListener<InitStateEvent>(eventSystem)
    , EventListener<DisconnectEvent>(eventSystem)
	, m_settings{ settings }
{
	m_networker.connect(m_settings.m_hostname, m_settings.m_port);
}

ConnectedState::~ConnectedState()
{
	if (m_map) {
		m_settings.m_username = m_map->username();
		m_settings.save();
	}
    m_renderer.clearStaticData();
}

void ConnectedState::update(double deltaTime, double timeStamp)
{
	m_networker.update(deltaTime, timeStamp);
	if (m_map != nullptr)
	{
		m_map->update(deltaTime, timeStamp);
	}
}

std::string ConnectedState::statusString() const {
	if (!m_networker.isConnected()) {
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
			if (m_map) {
				m_settings.m_username = m_map->username();
				m_settings.save();
			}
			m_game.openMainMenu();
		}
		
		ImGui::SameLine();
		ImGui::Text("%s", statusMessage.data());
	}
	ImGui::End();
}

void ConnectedState::draw(Renderer& renderer)
{
    if (m_map) {
        m_map->draw(renderer);
	}
	drawStatus();
}

using namespace std::placeholders;

void ConnectedState::receiveEvent(InitStateEvent* event) {
    m_map = std::make_unique<Map>(m_input, m_networker, m_eventSystem, event->m_config, m_settings.m_username);
    m_renderer.clearStaticData();
    m_map->drawGridStatic(m_renderer);
    m_map->drawWallsStatic(m_renderer, event->m_config.m_blockadePositions);
    m_map->updateGameState(event->m_state);
    m_map->m_lastUpdate -= event->m_timeSinceLastRound;

	m_eventSystem.processEvent(std::make_shared<SoundEvent>(SoundEnum::NextRound));
}

void ConnectedState::receiveEvent(DisconnectEvent* event) {
    m_map.release();
    m_renderer.clearStaticData();
}
