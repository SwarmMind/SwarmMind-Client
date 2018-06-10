#pragma once

#include <menu/MenuState.h>
#include <gamestate/Networker.h>
#include <gamestate/Map.h>
#include <input/Input.h>
#include <string>

class ConnectedState : public MenuState
{
public:
	ConnectedState(class Game& _game, class Sprites& _sprites, Input& input, class EventSystem& eventSystem, std::string address, unsigned port = 3000);
	virtual ~ConnectedState() override;


	virtual void update(double deltaTime) override;
	virtual void draw(Renderer& renderer) override;

	std::string statusString() const;
	void drawStatus();

	void onInitState(Configuration config, Gamestate *state);
	void onState(Gamestate *state);
	void onConnect();
	void onDisconnect();

	void enableCallbacks();

private:
	class Game& game;
	class Sprites& sprites;
	class EventSystem& eventSystem;
	Networker networker;
	Input& input;
	Map* map;
};
