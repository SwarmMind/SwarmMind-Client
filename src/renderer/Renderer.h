#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <renderer/Sprites.h>

class Camera;
struct Sprite;
class CommandVisualizer;

class Renderer
{
public:
	virtual Camera& camera() = 0;
    //Static draw
    virtual void addStaticSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite) = 0;
    virtual void clearStaticData() = 0;

    //Dynamic draw
	virtual void drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite) = 0;
	virtual void drawSprite(glm::vec3 pos, float width, float height, SpriteEnum sprite, glm::vec2 direction) = 0;
	virtual void drawCommandVisualizer(glm::vec3 position, CommandVisualizer& visualizer) = 0;
};
