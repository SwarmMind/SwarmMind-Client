#pragma once

#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>
#include <input/Input.h>
#include <string>

class Map
{
public:
	Map(class Input& _input, Networker& _networker, const class Configuration& config);

	~Map();

	void updateGameState(class Gamestate* newState);

	void update(double deltaTime);
	void draw(class Renderer& renderer);

private:
	class Gamestate* gamestate;
	class Input& input;
	class Networker& networker;
    class Configuration config;

	int selectedUnit = 0;
	bool selectedUnitIsValid();

	void updateSelection();

	void updateSelectionAction(Action action, int selectedPlayerNumber);

	void sendCommand(std::string action, std::string direction);
	void updateCommandAction(Action action, std::string command, std::string direction);
	void updateCommands();

    Textures tex;
    Sprites sprites;
};
