#include <renderer/Textures.h>
#include <renderer/Texture.h>
#include <stdexcept>

Textures::Textures()
{
	map<TextureEnum, string> paths = texturePaths();
	for (auto& path : paths)
	{
		textures[path.first] = new Texture(path.second);
	}
}

Textures::~Textures()
{
	for (auto& texture : textures)
	{
		delete texture.second;
	}
}

Texture * Textures::operator[](TextureEnum texture)
{
	return get(texture);
}

class Texture* Textures::get(TextureEnum texture)
{
	auto iterator = textures.find(texture);
	if (iterator == textures.end())
	{
		throw out_of_range("Texture is not loaded!");
	}
	return iterator->second;
}

std::map<TextureEnum, string> Textures::texturePaths()
{
	return map<TextureEnum, string>({
		{GridSpriteSheet, "res/gridSpriteSheet.png"},
        {UnitTexture, "res/unit.png"},
        {SelectionTexture, "res/selected.png"},
        {GridTexture, "res/grid.png"},
        {ChatTexture, "res/chat.png"},
        {WallTexture, "res/box.png"}
	});
}
