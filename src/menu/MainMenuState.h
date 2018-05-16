#pragma once

#include <menu/MenuState.h>

class MainMenuState : public MenuState
{
public:
	MainMenuState(class Game* _game);
	~MainMenuState();

	virtual void update(double deltaTime) override;
	virtual void draw(Renderer& renderer) override;
private:
	class Game* game;
};
