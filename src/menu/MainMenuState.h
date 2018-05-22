#pragma once

#include <menu/MenuState.h>

class MainMenuState : public MenuState
{
public:
	MainMenuState(class Game* _game);
	virtual ~MainMenuState() override;

	virtual void update(double deltaTime) override;
	virtual void draw(Renderer& renderer) override;
private:
	class Game* game;

	int addressBufferSize = 256;
	char address[256] = "localhost";
	
	int defaultPort = 3000;
	int port = defaultPort;
	int minimumPort = 0;
	int maximumPort = 65535;
};
