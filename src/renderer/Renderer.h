#pragma once

class Camera;
class Sprite;

class Renderer
{
public:
	virtual void drawSprite(float x, float y, float z, float width, float height, Sprite* sprite) = 0;
};
