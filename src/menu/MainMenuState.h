#pragma once

#include <menu/MenuState.h>

class MainMenuState : public MenuState
{
	const uint16_t default_port = 3000;

public:
	MainMenuState(class Game* _game, const std::string& preset_host = "localhost", const uint16_t preset_port = 3000);
	virtual ~MainMenuState() override;

	virtual void update(double deltaTime, double timeStamp) override;
	virtual void draw(Renderer& renderer) override;
private:
	class Game* game;

	int addressBufferSize = 256;
	char address[256];
	
	int port;
	int minimumPort = 0;
	int maximumPort = 65535;
};
