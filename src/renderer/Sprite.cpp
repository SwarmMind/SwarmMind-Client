#include <renderer/Sprite.h>
#include <renderer/Texture.h>


Sprite::Sprite(std::shared_ptr<Texture> texture, float u, float v, float uvWidth, float uvHeight)
	: m_texture{ texture }
	, m_u{ u }
	, m_v{ v }
	, m_u2{ u - uvWidth }
	, m_v2{ v - uvHeight }
{}

Sprite::~Sprite()
{}
