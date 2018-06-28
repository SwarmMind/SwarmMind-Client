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

Map::Map(Input& _input, Sprites& _sprites, Networker& _networker, EventSystem& _eventSystem, const Configuration& _config)
	: input{ _input }
	, config{_config}
	, networker{_networker}
	, gamestate{new Gamestate{_eventSystem}}
	, sprites{_sprites}
	, lastUpdate{glfwGetTime()}
	, eventSystem{_eventSystem}
    , m_chats{_input, _networker}
{
}

Map::~Map()
{
	delete gamestate;
}

void Map::updateGameState(class Gamestate* newState)
{
    delete gamestate;
	gamestate = newState;

	lastUpdate = glfwGetTime();
}

void Map::sendCommand(std::string action, glm::vec2 direction)
{
	auto unit = gamestate->units.find(selectedUnit);
	if (unit != gamestate->units.end()) {
		networker.sendCommand(unit->second.id(), action, direction);
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
	if (input.isActionJustPressed(action))
	{
		selectedUnit = clickedUnit(input.mousePositionInWorld());
		mouseClickPosition = input.mousePositionInWorld();
	}
	if (!isUnitClicked(mouseClickPosition))
	{
		return;
	}

	if (input.isActionJustReleased(action))
	{
		glm::vec2 delta = input.mousePositionInWorld() - mouseClickPosition;
		if (glm::length(delta) > minimumDragDistance)
		{
			sendCommand(command, glm::normalize(delta));
			ParticleSystem::spawnAcknowledgeParticles(input.mousePositionInWorld());
		}
	}

	if (input.isActionPressed(action))
	{
		glm::vec2 delta = input.mousePositionInWorld() - mouseClickPosition;
		if (glm::length(delta) > minimumDragDistance)
		{
			ParticleSystem::mouseDragParticles(input.mousePositionInWorld(), mouseClickPosition, action == Move ? glm::vec4(0.1, 0.8, 0.1, 0.7) : glm::vec4(1, 0.5, 0.1, 0.8), deltaTime);
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
	//updateCommandAction(Move, "move", "north");	

	updateCommandAction(ShootDown, "attack", "south");
	updateCommandAction(ShootUp, "attack", "north");
	updateCommandAction(ShootRight, "attack", "east");
	updateCommandAction(ShootLeft, "attack", "west");
	//updateCommandAction(Shoot, "shoot", "north");	
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
	std::map<uint32_t, Unit> units = gamestate->units;
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
	if (input.isActionJustPressed(action))
	{
		selectedUnit = selectedPlayerNumber;
	}
}

void Map::update(double deltaTime, double timeStamp)
{
	if (gamestate == nullptr)
		return;

    m_chats.update(deltaTime, timeStamp);
	gamestate->update(deltaTime);
	updateSelection();
	updateCommands(deltaTime);
}

void Map::drawGrid(Renderer& renderer) 
{
    const auto sprite = sprites.get(SpriteEnum::GridBlock);
	glm::vec3 p{ 0 };

    for (p.y = 0; p.y < config.sizeY; p.y++) {
		for (p.x = 0; p.x < config.sizeX; p.x++) {
			renderer.drawSprite(p, 1, 1, sprite);
		}
    }  
}

void Map::drawEntities(Renderer& renderer) 
{
	gamestate->draw(renderer);
}

void Map::draw(class Renderer& renderer)
{
    m_chats.draw(renderer);
    drawGrid(renderer);
    drawEntities(renderer);

	const auto units = gamestate->units;
	auto it = units.find(selectedUnit);
	if (it != units.cend())
	{
		renderer.drawSprite(glm::vec3{it->second.position() - glm::vec2(0.5f, 0.5f), 0.4}, 1, 1, sprites.get(SpriteEnum::SelectedBlock));
	}
}

bool Map::selectedUnitIsValid()
{
	return gamestate->units.find(selectedUnit) != gamestate->units.cend();
}
