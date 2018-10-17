#pragma once

#include <string>
#include <map>
#include <memory>

#include <menu/MenuState.h>
#include <gamestate/GameNetworker.h>
#include <gamestate/Map.h>
#include <input/Input.h>
#include <events/EventListener.h>
#include <events/InitStateEvent.h>
#include <events/StateEvent.h>
#include <events/DisconnectEvent.h>
#include <game/Settings.h>

class ConnectedState : public MenuState, public EventListener<InitStateEvent>, public EventListener<DisconnectEvent>
{
public:
    ConnectedState(class Game& game, class Renderer& renderer, Input& input, class EventSystem& eventSystem, Settings& settings);
	virtual ~ConnectedState() override;

	virtual void update(double deltaTime, double timeStamp) override;
	virtual void draw(Renderer& renderer) override;

	std::string statusString() const;
	void drawStatus();

    virtual void receiveEvent(InitStateEvent* event) override;
    virtual void receiveEvent(DisconnectEvent* event) override;
	
private:
	class Game& m_game;
    class Renderer& m_renderer;
	class EventSystem& m_eventSystem;
	GameNetworker m_networker;
	Input& m_input;
	std::unique_ptr<Map> m_map;
	Settings& m_settings;
};
