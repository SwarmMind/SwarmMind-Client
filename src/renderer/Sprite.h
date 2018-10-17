#pragma once

#include <memory>

#include <renderer/Texture.h>

struct Sprite
{
	Sprite(std::shared_ptr<Texture> texture, float u = 0, float v = 0, float uvWidth = 1, float uvHeight = 1);
	~Sprite();

	std::shared_ptr<Texture> m_texture;
	float m_u, m_v, m_u2, m_v2;
};
