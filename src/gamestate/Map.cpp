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

#include <glm/common.hpp>

using namespace std;

Map::Map(Input& _input, Sprites& _sprites, Networker& _networker, const Configuration& _config) 
	: input{ _input }
	, config{_config}
	, networker{_networker}
	, gamestate{new Gamestate{}}
    , old_gamestate{new Gamestate{}}
	, sprites{_sprites}
	, lastUpdate{glfwGetTime()}
{
}

Map::~Map()
{
	delete gamestate;
}

void Map::updateGameState(class Gamestate* newState)
{
	delete old_gamestate;
	old_gamestate = gamestate;
	gamestate = newState;

	lastUpdate = glfwGetTime();
}

void Map::sendCommand(std::string action, std::string direction)
{
	auto unit = gamestate->units.find(selectedUnit);
	if (unit != gamestate->units.end()) {
		networker.sendCommand(unit->second.id, action, direction);
	}
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
	EntityMap units = gamestate->units;
	for (auto it = units.cbegin(); it != units.cend(); it++) {
		if (glm::floor(mousePosition) == glm::vec2(it->second.posX, it->second.posY)) {
			return it->first;
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

void Map::drawGrid(Renderer& renderer) {
    const auto sprite = sprites.get(GridBlock);
    glm::vec3 p {0}; 

    for (p.y = 0; p.y < config.sizeY; p.y++) {
    for (p.x = 0; p.x < config.sizeX; p.x++) {
        renderer.drawSprite(p, 1, 1, sprite);
    }
    }  
}

void Map::drawEntities(Renderer& renderer) {
	const double t = glm::smoothstep(lastUpdate, lastUpdate + moveAnimationTime, glfwGetTime());

	{
	    const auto unitsprite = sprites.get(Unit);
	    const auto oldunits = old_gamestate->units, units = gamestate->units;
  
		for (auto it = units.cbegin(), oldit = oldunits.cbegin(); it != units.cend(); it++) {
			while (oldit != oldunits.cend() && oldit->first != it->first) { oldit++; }
	        if (oldit != oldunits.cend() && oldit->first == it->first) {
	            const glm::vec3 p { glm::mix(oldit->second.pos(), it->second.pos(), t), 1 };
	            renderer.drawSprite(p, 1, 1, unitsprite);
	        } else {
		        renderer.drawSprite(glm::vec3{it->second.pos(), 1}, 1, 1, unitsprite);
	        }
	    }
	}

	{
		const auto monstersprite = sprites.get(Monster);
	    const auto oldmonsters = old_gamestate->monsters, monsters = gamestate->monsters;

		for (auto it = monsters.cbegin(), oldit = oldmonsters.cbegin(); it != monsters.cend(); it++) {
			while (oldit != oldmonsters.cend() && oldit->first != it->first) { oldit++; }
	        if (oldit != oldmonsters.cend() && oldit->first == it->first) {
	            const glm::vec3 p { glm::mix(oldit->second.pos(), it->second.pos(), t), 1 };
	            renderer.drawSprite(p, 1, 1, monstersprite);
	        } else {
		        renderer.drawSprite(glm::vec3{it->second.pos(), 1}, 1, 1, monstersprite);
	        }
	    }
	}
}

void Map::draw(class Renderer& renderer)
{
    drawGrid(renderer);
    drawEntities(renderer);

	const auto units = gamestate->units;
	auto it = units.find(selectedUnit);
	if (it != units.cend())
	{
		renderer.drawSprite(glm::vec3{it->second.pos(), 0.4}, 1, 1, sprites.get(SelectedBlock));
	}
}

bool Map::selectedUnitIsValid()
{
	return gamestate->units.find(selectedUnit) != gamestate->units.cend();
}
