#pragma once
#include <map>
#include <string>

using namespace std;

enum TextureEnum
{
	PaprikaTexture,
	StarTexture
};

class Textures
{
public:
	Textures();

	~Textures();
	class Texture* operator[](TextureEnum texture);
	class Texture* get(TextureEnum texture);

private:
	map<TextureEnum, class Texture*> textures;
	map<TextureEnum, string> texturePaths();

};
