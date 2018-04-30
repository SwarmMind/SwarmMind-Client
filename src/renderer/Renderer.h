#pragma once

class Sprite;

class Renderer
{
public:
	virtual void drawSprite(float x, float y, float z, float width, float height, Sprite* sprite) = 0;
	virtual void setCamera(float x, float y, float height) = 0;
};
