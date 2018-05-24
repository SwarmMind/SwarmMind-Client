#include <iostream>
#include <cstdint>

#include <gamestate/Map.h>
#include <gamestate/Gamestate.h>
#include <input/Input.h>
#include <renderer/Renderer.h>
#include <imgui/imgui.h>
#include <renderer/Sprites.h>

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

void Map::updateCommands()
{
	if (!selectedUnitIsValid())
	{
		return;
	}
	
	updateCommandAction(MoveDown, "move", "south");
	updateCommandAction(MoveUp, "move", "north");
	updateCommandAction(MoveRight, "move", "east");
	updateCommandAction(MoveLeft, "move", "west");

	updateCommandAction(ShootDown, "shoot", "south");
	updateCommandAction(ShootUp, "shoot", "north");
	updateCommandAction(ShootRight, "shoot", "east");
	updateCommandAction(ShootLeft, "shoot", "west");
}

void Map::updateSelection()
{
	updateSelectionAction(SelectUnit1, 0);
	updateSelectionAction(SelectUnit2, 1);
	updateSelectionAction(SelectUnit3, 2);
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
	updateCommands();
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
