#pragma once

#include <glm/vec3.hpp>

class Camera;
class Sprite;
class CommandVisualizer;

class Renderer
{
public:
	virtual void drawSprite(glm::vec3 pos, float width, float height, Sprite* sprite) = 0;
	virtual void drawCommandVisualizer(glm::vec3 position, CommandVisualizer& visualizer) = 0;
};
