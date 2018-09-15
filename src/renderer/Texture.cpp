#include <renderer/Texture.h>
#include <lodepng/lodepng.h>
#include <stdexcept>

Texture::Texture(string fileName)
{
	vector<unsigned char> data = loadData(fileName);

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureID);
}

GLuint Texture::ID()
{
	return textureID;
}

std::vector<unsigned char> Texture::loadData(string fileName)
{
	vector<unsigned char> invertedData;
	unsigned int errorCode = lodepng::decode(invertedData, width, height, fileName);
	if (errorCode != 0)
	{
		throw runtime_error(string("Error while loading png: ") + string(fileName) + " Error message: " + lodepng_error_text(errorCode));
	}

	vector<unsigned char> correctedData(invertedData.size());
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			for (size_t c = 0; c < 4; c++)
			{
				correctedData.at((height - 1 - y) * width * 4 + 4 * x + c) = invertedData.at(y * width * 4 + 4 * x + c);
			}
		}
	}
	return correctedData;
}
