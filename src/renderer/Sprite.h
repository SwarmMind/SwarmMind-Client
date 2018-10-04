#pragma once

#include <memory>

#include <renderer/Texture.h>

struct Sprite
{
	Sprite(std::shared_ptr<Texture> texture, float u = 0, float v = 0, float uvWidth = 1, float uvHeight = 1);
	~Sprite();

	std::shared_ptr<Texture> texture;
	float u, v, u2, v2;
};
