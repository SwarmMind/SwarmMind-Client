#pragma once

class Map
{
public:
	Map(class Input& _input);

	~Map();

	void updateGameState(class Gamestate* newState);

	void draw(class Renderer& renderer);

private:
	class Gamestate* gamestate;
	class Input& input;
};
