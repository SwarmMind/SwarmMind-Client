#pragma once
#include <nlohmann/json.hpp>
#include <renderer/Renderer.h>
#include <gamestate/Networker.h>
#include <string>

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

	void drawConnecting();

	nlohmann::json* gameState;
	nlohmann::json stateConfiguration;
	Networker& networker;

};
