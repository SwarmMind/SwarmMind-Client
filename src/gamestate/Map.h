#pragma once

#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Networker.h>
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
	void updateCommands();

	void sendCommand(std::string action, std::string direction);

    Textures tex;
    Sprites sprites;
};
