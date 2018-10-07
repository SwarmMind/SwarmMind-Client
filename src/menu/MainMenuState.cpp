#include <menu/MainMenuState.h>

#include <algorithm>
#include <cstring>
#include <cstdint>

#include <imgui/imgui.h>
#include <imgui/misc/stl/imgui_stl.h>
#include <game/Game.h>
#include <gamestate/Map.h>

MainMenuState::MainMenuState(Game* _game, EventSystem& eventSystem, Input& input, Renderer& renderer, Settings& settings)
	: game{_game}
    , m_input{input}
    , m_renderer{renderer}
    , m_eventSystem{eventSystem}
    , m_networker(eventSystem)
    , EventListener<InitStateEvent>(eventSystem)
	, m_settings{settings}
{
	port_input = m_settings.port;
    m_networker.begin(m_renderer);
}

MainMenuState::~MainMenuState()
{

}

void MainMenuState::update(double deltaTime, double timeStamp)
{
    m_networker.update(deltaTime, timeStamp);
    if (m_map)
    {
        m_map->update(deltaTime, timeStamp);
    }
}

void MainMenuState::draw(Renderer& renderer)
{
    if (m_map)
    {
        m_map->draw(renderer);
    }

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
		if (ImGui::InputText("Address", &m_settings.hostname, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			game->connectTo(m_settings.hostname, m_settings.port);
            ImGui::End();
            return;
		}

		if (ImGui::Button("Connect!"))
		{
			game->connectTo(m_settings.hostname, m_settings.port);
            ImGui::End();
            return;
		}

		//For spacing only
		ImGui::Dummy(ImVec2(0, 10));

		if (ImGui::CollapsingHeader("Advanced Settings"))
		{
			if (ImGui::BeginChild("Advanced", ImVec2(0, 120), true))
			{
				if (ImGui::Button("Reset"))
				{
					m_settings.port = port_input = default_port;
				}
				ImGui::SameLine();
				if (ImGui::InputInt("Port", &port_input, 1, 100))
				{
					port_input = std::min(std::max(port_input, 0), USHRT_MAX);
					m_settings.port = static_cast<uint16_t>(port_input);
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::End();
}

void MainMenuState::receiveEvent(InitStateEvent* event)
{
    m_renderer.clearStaticData();
    m_map = std::make_unique<Map>(m_input, m_networker, m_eventSystem, event->m_config, m_settings.username);
    m_map->drawGridStatic(m_renderer);
    m_map->updateGameState(event->m_state);
    m_map->m_lastUpdate -= event->m_timeSinceLastRound;
}

