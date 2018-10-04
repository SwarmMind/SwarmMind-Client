#include <memory>
#include <stdexcept>

#include <renderer/Texture.h>
#include <renderer/Textures.h>

Textures::Textures()
{
	map<TextureEnum, string> paths = texturePaths();
	for (auto& path : paths)
	{
		textures[path.first] = std::make_shared<Texture>(path.second);
	}
}

Textures::~Textures()
{
}

std::shared_ptr<Texture> Textures::operator[](TextureEnum texture)
{
	return get(texture);
}

std::shared_ptr<Texture> Textures::get(TextureEnum texture)
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
		{MonsterTexture, "res/monster.png"},
        {UnitTexture, "res/unit.png"},
        {SelectionTexture, "res/selected.png"},
        {GridTexture, "res/grid.png"},
        {ChatTexture, "res/chat.png"},
        {WallTexture, "res/box.png"}
	});
}
