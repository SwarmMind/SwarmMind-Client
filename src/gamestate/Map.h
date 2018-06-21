#pragma once

#include <renderer/Textures.h>
#include <renderer/Sprites.h>
#include <gamestate/Gamestate.h>
#include <gamestate/Entity.h>
#include <gamestate/Networker.h>
#include <input/Input.h>
#include <game/Camera.h>
#include <string>
#include <glm/vec2.hpp>
#include <gamestate/ChatSystem.h>

class Map
{
public:
	Map(class Input& _input, class Sprites& _sprites, Networker& _networker, class EventSystem& _eventSystem, const class Configuration& config);

	~Map();

	void updateGameState(class Gamestate* newState);

	void update(double deltaTime);
	void draw(class Renderer& renderer);

	const double moveAnimationTime = 3.0;

private:
    ChatSystem m_chats;
	class Gamestate* gamestate;
	class Sprites& sprites;
	class Input& input;
	class Networker& networker;
	class EventSystem& eventSystem;
    class Configuration config;
	
	uint32_t selectedUnit = 0;
	bool selectedUnitIsValid();
	glm::vec2 mouseClickPosition;

	double lastUpdate;

	void updateSelection();
	void updateSelectionAction(Action action, int selectedPlayerNumber);
	bool isUnitClicked(glm::vec2 mousePosition);
	int clickedUnit(glm::vec2 mousePosition);

	void sendCommand(std::string action, glm::vec2 direction);
	void updateCommandAction(Action action, std::string command, std::string direction);
	void updateMouseCommand(Action action, std::string command, double deltaTime);
	void updateCommands(double deltaTime);

    void drawGrid(Renderer&);
    void drawEntities(Renderer&);
};
