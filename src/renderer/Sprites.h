#pragma once
#include <map>

using namespace std;

enum TextureEnum;

enum SpriteEnum
{
	PaprikaSprite,
	StarSprite
};

struct SpriteInitList {
	TextureEnum texture;
	float u, v, width, height;
};

class Sprites
{
public:
	Sprites(class Textures& textures);

	~Sprites();

	class Sprite* get(SpriteEnum sprite);

private:
	map<SpriteEnum, SpriteInitList> spriteTextures();
	map<SpriteEnum, class Sprite*> sprites;
};
