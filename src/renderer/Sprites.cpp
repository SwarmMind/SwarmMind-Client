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
		{ SpriteEnum::Monster, {GridSpriteSheet, 0, 0, 0.25, 0.25}},
        { SpriteEnum::WallBlock, {WallTexture, 0, 0, 1, 1 }},
        { SpriteEnum::ChatIcon, {ChatTexture, 0, 0, 1, 1}},
		{ SpriteEnum::SelectedBlock, {SelectionTexture, 0, 0, 1, 1}},
        { SpriteEnum::Unit, {UnitTexture, 0, 0, 1, 1} },
		{ SpriteEnum::GridBlock, {GridTexture, 0, 0, 0.5, 0.5}}
	});
}
