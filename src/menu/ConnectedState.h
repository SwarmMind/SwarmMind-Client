#pragma once

#include <menu/MenuState.h>
#include <gamestate/GameNetworker.h>
#include <gamestate/Map.h>
#include <input/Input.h>
#include <string>
#include <map>
#include <events/EventListener.h>
#include <events/InitStateEvent.h>
#include <events/StateEvent.h>
#include <events/DisconnectEvent.h>
#include <game/Settings.h>

class ConnectedState : public MenuState, public EventListener<InitStateEvent>, public EventListener<DisconnectEvent>
{
public:
    ConnectedState(class Game& _game, class Renderer& renderer, Input& input, class EventSystem& eventSystem, Settings& _settings);
	virtual ~ConnectedState() override;

	virtual void update(double deltaTime, double timeStamp) override;
	virtual void draw(Renderer& renderer) override;

	std::string statusString() const;
	void drawStatus();

    virtual void receiveEvent(InitStateEvent* event) override;
    virtual void receiveEvent(DisconnectEvent* event) override;
	
private:
	class Game& game;
    class Renderer& m_renderer;
	class EventSystem& eventSystem;
	GameNetworker networker;
	Input& input;
	Map* map;
	Settings& settings;
};
