#pragma once
#include <map>
#include <renderer/Textures.h>

using namespace std;

enum class SpriteEnum
{
	GridBlock,
	SelectedBlock,
	Unit,
	Monster,
    ChatIcon,
    WallBlock
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
