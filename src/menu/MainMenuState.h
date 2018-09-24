#pragma once

#include <game/Settings.h>
#include <menu/MenuState.h>
#include <gamestate/TutorialNetworker.h>
#include <events/EventSystem.h>
#include <events/EventListener.h>
#include <events/InitStateEvent.h>
#include <memory>

class MainMenuState : public MenuState, public EventListener<InitStateEvent>
{
	const uint16_t default_port = 3000;

public:
	MainMenuState(class Game* _game, class EventSystem& eventSystem, class Input& input, class Renderer& renderer, Settings& _settings);
	virtual ~MainMenuState() override;

	virtual void update(double deltaTime, double timeStamp) override;
	virtual void draw(class Renderer& renderer) override;

    virtual void receiveEvent(InitStateEvent* event) override;
private:
    class Renderer& m_renderer;
    TutorialNetworker m_networker;
    std::unique_ptr<class Map> m_map;

	class Game* game;
    class Input& m_input;
    class EventSystem& m_eventSystem;
	Settings& m_settings;

	int addressBufferSize = 256;
	char address[256];
	
	int port;
	int minimumPort = 0;
	int maximumPort = 65535;
};
