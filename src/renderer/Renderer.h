#pragma once

class Sprite;

struct Camera {
	float x;
	float y;
	float height;
	float width;
};

class Renderer
{
public:
	virtual void drawSprite(float x, float y, float z, float width, float height, Sprite* sprite) = 0;
	virtual void setCamera(float x, float y, float height) = 0;
	virtual Camera getCamera() = 0;
};
