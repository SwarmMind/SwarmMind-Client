#pragma once

#include <glm/vec3.hpp>

class Camera;
class Sprite;

class Renderer
{
public:
	virtual void drawSprite(glm::vec3 pos, float width, float height, Sprite* sprite) = 0;
};
