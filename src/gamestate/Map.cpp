#include <iostream>
#include <cstdint>
#include <limits>
#include <string>
#include <glm/glm.hpp>
#include <cmath>

#include <gamestate/Map.h>
#include <gamestate/Gamestate.h>
#include <input/Input.h>
#include <renderer/Renderer.h>
#include <imgui/imgui.h>
#include <renderer/Sprites.h>
#include <renderer/ParticleSystem.h>

#include <glm/common.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <renderer/CommandVisualizer.h>
#include <events/EventSystem.h>

using namespace std;

Map::Map(Input& _input, Networker& _networker, EventSystem& _eventSystem, const Configuration& _config)
	: EventListener<StateEvent>{_eventSystem}
    , EventListener<AccumulatedCommandsEvent>{_eventSystem}
    , m_input{ _input }
	, m_config{_config}
	, m_networker{_networker}
    , m_gamestate{ new Gamestate{_eventSystem} }
	, m_lastUpdate{glfwGetTime()}
	, m_eventSystem{_eventSystem}
    , m_chats{_input, _networker, _eventSystem}
    , m_roundDuration{_config.m_roundTime}
{}

Map::~Map()
{
	delete m_gamestate;
}

void Map::updateGameState(class Gamestate* newState)
{
    delete m_gamestate;
	m_gamestate = newState;

	m_lastUpdate = glfwGetTime();
    m_numberOfGivenCommands = 0;
}

void Map::receiveEvent(StateEvent* event)
{
    updateGameState(event->m_state);
    event->m_state = nullptr;
}

void Map::receiveEvent(AccumulatedCommandsEvent* event)
{
    m_numberOfGivenCommands = event->numberOfGivenCommands;
    m_maxNumberOfCommands = event->maxNumberOfCommands;
}

void Map::sendCommand(std::string action, glm::vec2 direction)
{
	auto unit = m_gamestate->units.find(m_selectedUnit);
	if (unit != m_gamestate->units.end()) {
		m_networker.sendCommand(unit->second.id(), action, direction);
	}
}

void Map::updateCommandAction(Action action, std::string command, std::string direction)
{
	//#TODO
	/*if (input.isActionJustReleased(action))
	{
		sendCommand(command, direction);
	}*/
}

void Map::updateMouseCommand(Action action, std::string command, double deltaTime)
{
	float minimumDragDistance = 0.5;
	if (m_input.isActionJustPressed(action))
	{
		m_selectedUnit = clickedUnit(m_input.mousePositionInWorld());
		m_mouseClickPosition = m_input.mousePositionInWorld();
	}
	if (!isUnitClicked(m_mouseClickPosition))
	{
		return;
	}

	if (m_input.isActionJustReleased(action))
	{
		glm::vec2 delta = m_input.mousePositionInWorld() - m_mouseClickPosition;
		if (glm::length(delta) > minimumDragDistance)
		{
			sendCommand(command, glm::normalize(delta));
			ParticleSystem::spawnAcknowledgeParticles(m_input.mousePositionInWorld());
		}
	}

	if (m_input.isActionPressed(action))
	{
		glm::vec2 delta = m_input.mousePositionInWorld() - m_mouseClickPosition;
		if (glm::length(delta) > minimumDragDistance)
		{
			ParticleSystem::mouseDragParticles(m_input.mousePositionInWorld(), m_mouseClickPosition, action == Move ? glm::vec4(0.1, 0.8, 0.1, 0.7) : glm::vec4(1, 0.5, 0.1, 0.8), deltaTime);
		}
	}
}

void Map::updateCommands(double deltaTime)
{
	updateMouseCommand(Move, "move", deltaTime);
	updateMouseCommand(Shoot, "attack", deltaTime);

	if (!selectedUnitIsValid())
	{
		return;
	}
	
	updateCommandAction(MoveDown, "move", "south");
	updateCommandAction(MoveUp, "move", "north");
	updateCommandAction(MoveRight, "move", "east");
	updateCommandAction(MoveLeft, "move", "west");

	updateCommandAction(ShootDown, "attack", "south");
	updateCommandAction(ShootUp, "attack", "north");
	updateCommandAction(ShootRight, "attack", "east");
	updateCommandAction(ShootLeft, "attack", "west");
}

void Map::updateSelection()
{
	updateSelectionAction(SelectUnit1, 0);
	updateSelectionAction(SelectUnit2, 1);
	updateSelectionAction(SelectUnit3, 2);
}

bool Map::isUnitClicked(glm::vec2 mousePosition) {
	return clickedUnit(mousePosition) != -1;
}

int Map::clickedUnit(glm::vec2 mousePosition)
{
	std::map<uint32_t, Unit> units = m_gamestate->units;
	auto closest = units.cend();
	float actualDistance = std::numeric_limits<float>::infinity();

	for (auto it = units.cbegin(); it != units.cend(); it++) {
		float distance = glm::distance(it->second.position(), mousePosition);
		if (distance < actualDistance) {
			actualDistance = distance;
			closest = it;
		}
	}
	return (closest == units.cend() || actualDistance > 0.5f) ? -1 : closest->first;
}

void Map::updateSelectionAction(Action action, int selectedPlayerNumber)
{
	if (m_input.isActionJustPressed(action))
	{
		m_selectedUnit = selectedPlayerNumber;
	}
}

void Map::update(double deltaTime, double timeStamp)
{
	if (m_gamestate == nullptr)
		return;

    m_chats.update(deltaTime, timeStamp);
	m_gamestate->update(deltaTime);
	
    updateSelection();
	updateCommands(deltaTime);
}

void Map::drawGridStatic(Renderer& renderer) 
{
	glm::vec3 p{ 0 };

    for (p.y = 0; p.y < m_config.m_sizeY; p.y++) {
		for (p.x = 0; p.x < m_config.m_sizeX; p.x++) {
			renderer.addStaticSprite(p, 1, 1, SpriteEnum::GridBlock);
		}
    }
}

void Map::drawEntities(Renderer& renderer) 
{
	m_gamestate->draw(renderer);
}

void Map::draw(class Renderer& renderer)
{
    m_chats.draw(renderer);
    //drawGridStatic(renderer);
    drawEntities(renderer);

	const auto units = m_gamestate->units;
	auto it = units.find(m_selectedUnit);
	if (it != units.cend())
	{
		renderer.drawSprite(glm::vec3{it->second.position() - glm::vec2(0.5f, 0.5f), 0.4}, 1, 1, SpriteEnum::SelectedBlock);
	}

    drawRoundProgress();
}

void Map::drawRoundProgress()
{
    ImVec2 imGuiDisplaySize = ImGui::GetIO().DisplaySize;
    ImVec2 position(imGuiDisplaySize.x / 2.0, imGuiDisplaySize.y - 30);
    ImGui::SetNextWindowPos(position, ImGuiCond_Always, ImVec2(0.5, 1.0));
    ImGui::SetNextWindowSize(ImVec2(imGuiDisplaySize.x / 2.0, 0.0f), ImGuiCond_Always);
    if (ImGui::Begin("RoundProgressWindow", nullptr,
        ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoInputs))
    {
        ImGui::TextUnformatted("Round Progress");

        ImGui::ProgressBar(static_cast<double>(m_numberOfGivenCommands) / m_maxNumberOfCommands, ImVec2(-1, 0), "");
        ImGui::SameLine(ImGui::GetTextLineHeightWithSpacing());
        ImGui::TextUnformatted("Commands");


        ImGui::ProgressBar((glfwGetTime() - m_lastUpdate) / m_roundDuration, ImVec2(-1, 0), "");
        ImGui::SameLine(ImGui::GetTextLineHeightWithSpacing());
        ImGui::TextUnformatted("Time");
    }
    ImGui::End();
}

bool Map::selectedUnitIsValid()
{
	return m_gamestate->units.find(m_selectedUnit) != m_gamestate->units.cend();
}
