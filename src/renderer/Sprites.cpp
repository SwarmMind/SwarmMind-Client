#include <renderer/Sprites.h>

#include <stdexcept>

#include <renderer/Textures.h>
#include <renderer/Texture.h>
#include <renderer/Sprite.h>

Sprites::Sprites(class Textures& textures)
{
	auto spriteList = spriteTextures();
	for (auto& spriteInit : spriteList)
	{
		SpriteInitList initList = spriteInit.second;
		sprites[spriteInit.first] = new Sprite(
			textures[initList.texture],
			initList.u,
			initList.v,
			initList.width,
			initList.height
		);
	}
}

Sprites::~Sprites()
{
	for (auto& sprite : sprites)
	{
		delete sprite.second;
	}
}

class Sprite* Sprites::get(SpriteEnum sprite)
{
	auto iterator = sprites.find(sprite);
	if (iterator == sprites.end())
	{
		throw out_of_range("Sprite is not loaded!");
	}
	else
	{
		return iterator->second;
	}
}

std::map <SpriteEnum, SpriteInitList> Sprites::spriteTextures()
{
	return map<SpriteEnum, SpriteInitList>({
		{ SpriteEnum::GridBlock, {GridSpriteSheet, 0, 0.25, 0.25, 0.25}},
		{ SpriteEnum::SelectedBlock, {GridSpriteSheet, 0.25, 0, 0.25, 0.25}},
		{ SpriteEnum::Unit, {GridSpriteSheet, 0.25, 0.25, 0.25, 0.25}},
		{ SpriteEnum::Monster, {GridSpriteSheet, 0, 0, 0.25, 0.25}},
        { SpriteEnum::ChatIcon, { GridSpriteSheet, 0.5, 0.25, 0.25, 0.25}}
	});
}
