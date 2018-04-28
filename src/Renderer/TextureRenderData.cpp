#include <Renderer/TextureRenderData.h>

TextureRenderData::TextureRenderData()
{
	vertexBufferSize = 1024 * 1024;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(GLfloat), nullptr, GL_MAP_WRITE_BIT);
	mappedBuffer = nullptr;
	bufferOffset = 0;
}

TextureRenderData::~TextureRenderData()
{
	glDeleteBuffers(1, &vertexBuffer);
}

bool TextureRenderData::addData(unsigned int size, GLfloat* data)
{
	if (mappedBuffer == nullptr)
	{
		mapBuffer();
	}

	if (vertexBufferSize - bufferOffset < size)
	{
		return false;
	}
	else
	{
		memcpy(mappedBuffer + bufferOffset, data, size * sizeof(GLfloat));
		bufferOffset += size;
		return true;
	}
}

void TextureRenderData::mapBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	mappedBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	bufferOffset = 0;
}

void TextureRenderData::unmapBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	mappedBuffer = nullptr;
	bufferOffset = 0;
}

