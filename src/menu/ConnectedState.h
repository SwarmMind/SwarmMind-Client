#pragma once

#include <menu/MenuState.h>
#include <gamestate/Networker.h>
#include <gamestate/Map.h>
#include <input/Input.h>
#include <string>
#include <map>
#include <events/EventListener.h>
#include <events/InitStateEvent.h>
#include <events/StateEvent.h>
#include <events/DisconnectEvent.h>

class ConnectedState : public MenuState, public EventListener<InitStateEvent>, public EventListener<StateEvent>, public EventListener<DisconnectEvent>
{
public:
    ConnectedState(class Game& _game, class Sprites& _sprites, Input& input, class EventSystem& eventSystem, std::string address, unsigned port = 3000);
	virtual ~ConnectedState() override;

	virtual void update(double deltaTime, double timeStamp) override;
	virtual void draw(Renderer& renderer) override;

	std::string statusString() const;
	void drawStatus();

	virtual void receiveEvent(StateEvent* event) override;
    virtual void receiveEvent(InitStateEvent* event) override;
    virtual void receiveEvent(DisconnectEvent* event) override;
	
private:
	class Game& game;
	class Sprites& sprites;
	class EventSystem& eventSystem;
	Networker networker;
	Input& input;
	Map* map;
};
