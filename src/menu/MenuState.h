#pragma once

#include <renderer/Renderer.h>

class MenuState
{
public:
	MenuState() = default;
	virtual ~MenuState() = default;

	virtual void update(double deltaTime) = 0;
	virtual void draw(Renderer& renderer) = 0;
private:
};
