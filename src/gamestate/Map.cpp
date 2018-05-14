#include <iostream>
#include <cstdint>

#include <gamestate/Map.h>
#include <gamestate/Gamestate.h>
#include <input/Input.h>
#include <renderer/Renderer.h>
#include <imgui/imgui.h>
#include <renderer/Sprites.h>

Map::Map(Input& _input, Networker& _networker, const Configuration& _config) 
	: input{ _input }
	, config{_config}
	, networker{_networker}
	, gamestate{nullptr}
	, sprites{tex}
{

}

Map::~Map()
{
	delete this->gamestate;
}

void Map::updateGameState(class Gamestate* newState)
{
	delete this->gamestate;
	this->gamestate = newState;
}

void Map::sendCommand(std::string action, std::string direction)
{
	if (!this->selectedUnitIsValid())
	{
		return;
	}

	Entity unit = this->gamestate->getUnits().at(this->selectedUnit);
	networker.sendCommand(unit.id, action, direction);
}

void Map::updateCommandAction(Action action, std::string command, std::string direction)
{
	if (input.isActionJustReleased(action))
	{
		this->sendCommand(command, direction);
	}
}

void Map::updateCommands()
{
	if (!this->selectedUnitIsValid())
	{
		return;
	}
	
	this->updateCommandAction(MoveDown, "move", "south");
	this->updateCommandAction(MoveUp, "move", "north");
	this->updateCommandAction(MoveRight, "move", "east");
	this->updateCommandAction(MoveLeft, "move", "west");

	this->updateCommandAction(ShootDown, "shoot", "south");
	this->updateCommandAction(ShootUp, "shoot", "north");
	this->updateCommandAction(ShootRight, "shoot", "east");
	this->updateCommandAction(ShootLeft, "shoot", "west");
}

void Map::updateSelection()
{
	if (input.isActionJustPressed(SelectUnit1))
	{
		this->selectedUnit = 0;
	}
	if (input.isActionJustPressed(SelectUnit2))
	{
		this->selectedUnit = 1;
	}
	if (input.isActionJustPressed(SelectUnit3))
	{
		this->selectedUnit = 2;
	}
}

void Map::update(double deltaTime)
{
	if (this->gamestate == nullptr)
		return;

	this->updateSelection();
	this->updateCommands();
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
	if (this->selectedUnitIsValid())
	{
		renderer.drawSprite(units[selectedUnit].posX, units[selectedUnit].posY, 0.5, 1, 1, sprites.get(SelectedBlock));
	}
}

bool Map::selectedUnitIsValid()
{
	vector<Entity> units = gamestate->getUnits();
	return selectedUnit >= 0 && selectedUnit < units.size();
}
