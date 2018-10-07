#include <renderer/Sprite.h>
#include <renderer/Texture.h>


Sprite::Sprite(std::shared_ptr<Texture> texture, float u, float v, float uvWidth, float uvHeight)
	: texture{ texture }
	, u{ u }
	, v{ v }
	, u2{ u - uvWidth }
	, v2{ v - uvHeight }
{}

Sprite::~Sprite()
{}
