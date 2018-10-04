#pragma once

#include <map>
#include <memory>

#include <renderer/Textures.h>
#include <renderer/Sprite.h>

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

	std::shared_ptr<Sprite> get(SpriteEnum sprite);

private:
	map<SpriteEnum, SpriteInitList> spriteTextures();
	map<SpriteEnum, std::shared_ptr<Sprite>> sprites;
};
