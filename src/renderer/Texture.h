#pragma once

#include <glbinding/gl41core/gl.h>
#include <string>
#include <vector>

using namespace gl41core;
using namespace std;

class Texture
{
public:
	Texture(string fileName);

	~Texture();

	GLuint ID();

private:
	vector<unsigned char> loadData(string fileName);

	GLuint textureID;
	unsigned int width, height;
};
