#pragma once
#include <map>
#include <string>
#include <memory>

#include <renderer/Texture.h>

using namespace std;

enum TextureEnum
{
	MonsterTexture,
    UnitTexture,
    SelectionTexture,
    GridTexture,
    ChatTexture,
    WallTexture
};

class Textures
{
public:
	Textures();

	~Textures();
	std::shared_ptr<Texture> operator[](TextureEnum texture);
	std::shared_ptr<Texture> get(TextureEnum texture);

private:
	map<TextureEnum, std::shared_ptr<Texture>> m_textures;
	static const map<TextureEnum, string> m_texturePaths;
};
