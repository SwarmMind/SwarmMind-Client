#pragma once

#include <glbinding/gl41core/gl.h>
#include <string>
#include <vector>

using namespace gl41core;
using namespace std;

using TextureID = GLuint;

class Texture
{
public:
	Texture(string fileName);

	~Texture();

	TextureID ID();

private:
	vector<unsigned char> loadData(string fileName);

	GLuint m_textureID;
	unsigned int m_width, m_height;
};
