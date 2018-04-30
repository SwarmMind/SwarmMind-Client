#include <renderer/Sprite.h>
#include <renderer/Texture.h>


Sprite::Sprite(Texture* texture, float u, float v, float uvWidth, float uvHeight)
	: _texture{ texture }
	, _u{ u }
	, _v{ v }
	, _u2{ u + uvWidth }
	, _v2{ v + uvHeight }
{}

Sprite::~Sprite()
{}

void Sprite::texture(class Texture* val)
{
	_texture = val;
}

class Texture* Sprite::texture() const
{
	return _texture;
}

void Sprite::u(float val)
{
	_u = val;
}

float Sprite::u() const
{
	return _u;
}

void Sprite::v(float val)
{
	_v = val;
}

float Sprite::v() const
{
	return _v;
}

void Sprite::u2(float val)
{
	_u2 = val;
}

float Sprite::u2() const
{
	return _u2;
}

void Sprite::v2(float val)
{
	_v2 = val;
}

float Sprite::v2() const
{
	return _v2;
}
