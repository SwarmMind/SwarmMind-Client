#include <iostream>
#include <cstdint>
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

using namespace std;

Map::Map(Input& _input, Sprites& _sprites, Networker& _networker, const Configuration& _config) 
	: input{ _input }
	, config{_config}
	, networker{_networker}
	, gamestate{nullptr}
	, sprites{_sprites}
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
}

void Map::sendCommand(std::string action, std::string direction)
{
	if (!selectedUnitIsValid())
	{
		return;
	}

	Entity unit = gamestate->getUnits().at(selectedUnit);
	networker.sendCommand(unit.id, action, direction);
}

void Map::updateCommandAction(Action action, std::string command, std::string direction)
{
	if (input.isActionJustReleased(action))
	{
		sendCommand(command, direction);
	}
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
			if (std::abs(delta.x) > std::abs(delta.y)) {
				sendCommand(command, delta.x > 0 ? "east" : "west");
			}
			else {
				sendCommand(command, delta.y > 0 ? "north" : "south");
			}
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
	updateMouseCommand(Shoot, "shoot", deltaTime);

	if (!selectedUnitIsValid())
	{
		return;
	}
	
	updateCommandAction(MoveDown, "move", "south");
	updateCommandAction(MoveUp, "move", "north");
	updateCommandAction(MoveRight, "move", "east");
	updateCommandAction(MoveLeft, "move", "west");
	//updateCommandAction(Move, "move", "north");	

	updateCommandAction(ShootDown, "shoot", "south");
	updateCommandAction(ShootUp, "shoot", "north");
	updateCommandAction(ShootRight, "shoot", "east");
	updateCommandAction(ShootLeft, "shoot", "west");
	//updateCommandAction(Shoot, "shoot", "north");	
}

void Map::updateSelection()
{
	updateSelectionAction(SelectUnit1, 0);
	updateSelectionAction(SelectUnit2, 1);
	updateSelectionAction(SelectUnit3, 2);
}

glm::vec2 Map::getCellOfMousePosition() 
{
	return floor(input.mousePositionInWorld());
}

bool Map::isUnitClicked(glm::vec2 mousePosition) {
	return clickedUnit(mousePosition) != -1;
}

int Map::clickedUnit(glm::vec2 mousePosition)
{
	vector<Entity> units = gamestate->getUnits();
	for (int i = 0; i < units.size(); i++) {
		Entity& unit = units.at(i);
		if (glm::floor(mousePosition) == glm::vec2(unit.posX, unit.posY)) {
			return i;
		}
	}
	return -1;
}

void Map::updateSelectionAction(Action action, int selectedPlayerNumber)
{
	if (input.isActionJustPressed(action))
	{
		selectedUnit = selectedPlayerNumber;
	}
}

void Map::update(double deltaTime)
{
	if (gamestate == nullptr)
		return;

	updateSelection();
	updateCommands(deltaTime);
}

void Map::draw(class Renderer& renderer)
{
    if (gamestate == nullptr) return;
    for (uint32_t y = 0; y < config.sizeY; y++) {
    for (uint32_t x = 0; x < config.sizeX; x++) {
        renderer.drawSprite(x, y, 0, 1, 1, sprites.get(GridBlock));
    }
    }

    for (const auto& unit: gamestate->getUnits()) {
        renderer.drawSprite(unit.posX, unit.posY, 1, 1, 1, sprites.get(Unit));
    }

    for (const auto& monster: gamestate->getMonsters()) {
        renderer.drawSprite(monster.posX, monster.posY, 1, 1, 1, sprites.get(Monster));
    }
	vector<Entity> units = gamestate->getUnits();
	if (selectedUnitIsValid())
	{
		renderer.drawSprite(units[selectedUnit].posX, units[selectedUnit].posY, 0.4, 1, 1, sprites.get(SelectedBlock));
	}
}

bool Map::selectedUnitIsValid()
{
	vector<Entity> units = gamestate->getUnits();
	return selectedUnit >= 0 && selectedUnit < units.size();
}
