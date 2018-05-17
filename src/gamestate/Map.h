#pragma once

#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <gamestate/Gamestate.h>

class Map
{
public:
	Map(class Input& _input, const class Configuration& config);

	~Map();

	void updateGameState(class Gamestate* newState);

	void draw(class Renderer& renderer);

private:
	class Gamestate* gamestate;
	class Input& input;
	class Configuration config;

	Textures tex;
	Sprites sprites;
};
