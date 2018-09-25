#include <menu/MainMenuState.h>

#include <imgui/imgui.h>
#include <game/Game.h>
#include <algorithm>
#include <cstring>
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
	strcpy(address, m_settings.hostname.c_str());
	port = m_settings.port;

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
		if (ImGui::InputText("Address", address, addressBufferSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			game->connectTo(address, port);
            ImGui::End();
            return;
		}

		if (ImGui::Button("Connect!"))
		{
			game->connectTo(address, port);
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

void MainMenuState::receiveEvent(InitStateEvent* event)
{
    m_renderer.clearStaticData();
    m_map = std::make_unique<Map>(m_input, m_networker, m_eventSystem, event->m_config, m_settings.username);
    m_map->drawGridStatic(m_renderer);
    m_map->updateGameState(event->m_state);
    m_map->m_lastUpdate -= event->m_timeSinceLastRound;
}

