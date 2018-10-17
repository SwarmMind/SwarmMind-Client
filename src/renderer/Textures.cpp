#include <memory>
#include <stdexcept>

#include <renderer/Texture.h>
#include <renderer/Textures.h>

Textures::Textures()
{
	for (auto& path : m_texturePaths)
	{
		m_textures[path.first] = std::make_shared<Texture>(path.second);
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
	auto iterator = m_textures.find(texture);
	if (iterator == m_textures.end())
	{
		throw out_of_range("Texture is not loaded!");
	}
	return iterator->second;
}

const std::map<TextureEnum, string> Textures::m_texturePaths = {
		{MonsterTexture, "res/monster.png"},
        {UnitTexture, "res/unit.png"},
        {SelectionTexture, "res/selected.png"},
        {GridTexture, "res/grid.png"},
        {ChatTexture, "res/chat.png"},
        {WallTexture, "res/box.png"}
};
