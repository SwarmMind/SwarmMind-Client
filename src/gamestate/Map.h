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
#include <events/StateEvent.h>
#include <events/AccumulatedCommandsEvent.h>
#include <sound/Sounds.h>

class Map : public EventListener<StateEvent>, public EventListener<AccumulatedCommandsEvent>
{
public:
	Map(class Input& _input, Networker& _networker, class EventSystem& _eventSystem, const class Configuration& config, const std::string& preset_username);

	~Map();

	void updateGameState(class Gamestate* newState);
	void update(double deltaTime, double timeStamp);

    void drawGridStatic(Renderer&);
	void draw(class Renderer& renderer);
    void drawWallsStatic(class Renderer& renderer, std::vector<glm::vec2> blockadePositions);	

    virtual void receiveEvent(StateEvent* event) override;
    virtual void receiveEvent(AccumulatedCommandsEvent* event) override;

    std::string username() const { return m_chats.username(); }

    const double moveAnimationTime = 3.0;
	double m_lastUpdate;	 

protected:
    ChatSystem m_chats;
	class Gamestate* m_gamestate;
	class Input& m_input; 
	class Networker& m_networker;
	class EventSystem& m_eventSystem;
    Configuration m_config;
    
    bool trackpadMode = true;

	int32_t m_selectedUnit = 0;
	bool selectedUnitIsValid();
	glm::vec2 m_mouseClickPosition;

    double m_roundDuration = 1.0; //set to 1 to avoid divide by zero error
    size_t m_numberOfGivenCommands = 0;
    size_t m_maxNumberOfCommands = 1; //set to 1 to avoid divide by zero error

	void updateSelection();
	void updateSelectionAction(Action action, int selectedPlayerNumber);
	bool isUnitClicked(glm::vec2 mousePosition);
	int clickedUnit(glm::vec2 mousePosition);

	void sendCommand(std::string action, glm::vec2 direction);
	void updateCommandAction(Action action, std::string command, std::string direction);
	void updateMouseCommand(Action action, std::string command, double deltaTime, bool isDirect);
	void updateCommands(double deltaTime);

    void drawRoundProgress();
    void drawEntities(Renderer&);
};
