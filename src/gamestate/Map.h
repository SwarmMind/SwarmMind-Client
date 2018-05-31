#pragma once

#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>
#include <input/Input.h>
#include <game/Camera.h>
#include <string>
#include <glm/vec2.hpp>

class Map
{
public:
	Map(class Input& _input, class Sprites& _sprites, Networker& _networker, const class Configuration& config);

	~Map();

	void updateGameState(class Gamestate* newState);

	void update(double deltaTime);
	void draw(class Renderer& renderer);

private:
	class Gamestate* gamestate;
	class Sprites& sprites;
	class Input& input;
	class Networker& networker;
    class Configuration config;
	
	int selectedUnit = 0;
	bool selectedUnitIsValid();
	glm::vec2 mouseClickPosition;

	void updateSelection();
	glm::vec2 getCellOfMousePosition();
	void updateSelectionAction(Action action, int selectedPlayerNumber);
	bool isUnitClicked(glm::vec2 mousePosition);
	int clickedUnit(glm::vec2 mousePosition);

	void sendCommand(std::string action, std::string direction);
	void updateCommandAction(Action action, std::string command, std::string direction);
	void updateMouseCommand(Action action, std::string command, double deltaTime);
	void updateCommands(double deltaTime);
};
