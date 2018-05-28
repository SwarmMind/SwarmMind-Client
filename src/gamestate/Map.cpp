#include <iostream>
#include <cstdint>

#include <gamestate/Map.h>
#include <gamestate/Gamestate.h>
#include <input/Input.h>
#include <renderer/Renderer.h>
#include <imgui/imgui.h>
#include <renderer/Sprites.h>

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

void Map::updateMouseCommand(Action action, std::string command)
{
	string direction;
	if (input.isActionJustReleased(action))
	{
		sendCommand(command, direction);
	}
}

string Map::getDirection() {
	return "nichts";
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
	updateCommandAction(Move, "move", "north");
	//updateMouseCommand(Move, "move");

	updateCommandAction(ShootDown, "shoot", "south");
	updateCommandAction(ShootUp, "shoot", "north");
	updateCommandAction(ShootRight, "shoot", "east");
	updateCommandAction(ShootLeft, "shoot", "west");
	updateCommandAction(Shoot, "shoot", "north");
	//updateMouseCommand(Shoot, "shoot");
}

void Map::updateSelection()
{
	updateSelectionAction(SelectUnit1, 0);
	updateSelectionAction(SelectUnit2, 1);
	updateSelectionAction(SelectUnit3, 2);
	updateMouseSelection(SelectUnit);
}

int Map::getSelectedUnit(){

	//doesUnitExist() is missing
/*
	Entity unit1 = gamestate->getUnits().at(0);
	Entity unit2 = gamestate->getUnits().at(1);
	Entity unit3 = gamestate->getUnits().at(2);

	cout << "Mouse_x: " << input.getMousePosition("x") << endl;
	cout << "Mouse_Y: " << input.getMousePosition("y") << endl;
	cout << "Unit2_x: " << unit2.posX << endl;
	cout << "Unit2_y: " << unit2.posY << endl;

	if (input.getMousePosition("x") == unit1.posX && input.getMousePosition("y") == unit1.posY) { // comparing double with unsigned??
		selectedUnit = 0;
	}
	if (input.getMousePosition("x") == unit2.posX && input.getMousePosition("y") == unit2.posY) { // comparing double with unsigned??
		selectedUnit = 1;
	}
	if (input.getMousePosition("x") == unit3.posX && input.getMousePosition("y") == unit3.posY) { // comparing double with unsigned??
		selectedUnit = 2;
	}
	else {
		selectedUnit = -1;
	}
*/
	return 2;
}

void Map::updateMouseSelection(Action action) {

	if (input.isActionJustPressed(action))
	{
		selectedUnit = getSelectedUnit();
	}
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
		renderer.drawSprite(units[selectedUnit].posX, units[selectedUnit].posY, 0.5, 1, 1, sprites.get(SelectedBlock));
	}
}

bool Map::selectedUnitIsValid()
{
	vector<Entity> units = gamestate->getUnits();
	return selectedUnit >= 0 && selectedUnit < units.size();
}
