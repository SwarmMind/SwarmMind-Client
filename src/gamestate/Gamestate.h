#pragma once
#include <nlohmann/json.hpp>
#include <renderer/Renderer.h>
#include <gamestate/Networker.h>
#include <string>
#include <renderer/Sprites.h>
#include <renderer/Textures.h>

class Gamestate
{
public:
	Gamestate(Networker& _networker);
	~Gamestate();

	void update(double deltaTime);
	void draw(Renderer& renderer);

private:
	void updateConfiguration();
	void updateGameState();

	void drawStatus(std::string statusMessage);
	void drawState(Renderer& renderer);

	nlohmann::json* gameState;
	nlohmann::json stateConfiguration;
	Networker& networker;
	Textures textures;
	Sprites sprites;
};
