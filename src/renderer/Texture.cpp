#include <renderer/Texture.h>
#include <lodepng/lodepng.h>
#include <stdexcept>
#include <iostream>

Texture::Texture(string fileName)
{
	vector<unsigned char> data = loadData(fileName);

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureID);
}

GLuint Texture::ID()
{
	return m_textureID;
}

std::vector<unsigned char> Texture::loadData(string fileName)
{
    std::cout << "Loading texture: \"" << fileName << "\"... " << std::flush;
	vector<unsigned char> data;
	unsigned int errorCode = lodepng::decode(data, m_width, m_height, fileName);
	if (errorCode != 0)
	{
		throw runtime_error(string("Error while loading png: ") + string(fileName) + " Error message: " + lodepng_error_text(errorCode));
	}
    std::cout << "[done]" << std::endl;

	return data;
}
