#include <renderer/TextureRenderData.h>
#include <cstring>

using namespace std;

TextureRenderData::TextureRenderData()
{
	vertexBufferSize = 512 * 1024;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(GLfloat), nullptr, GL_MAP_WRITE_BIT);
	
	glGenBuffers(1, &secondBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, secondBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(GLfloat), nullptr, GL_MAP_WRITE_BIT);

	mappedBuffer = nullptr;
	bufferOffset = 0;
}

TextureRenderData::~TextureRenderData()
{
	this->unmapBuffer();
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &secondBuffer);
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

void TextureRenderData::swapBuffers()
{
	//Optimization
	//This will keep the same buffer from getting remapped immediately after being unmapped and used for drawing
	//Therefore glMapBuffer will not block because it will map the buffer currently not used for drawing
	swap(vertexBuffer, secondBuffer);
}

void TextureRenderData::mapBuffer()
{
	swapBuffers();
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	mappedBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	bufferOffset = 0;
}

void TextureRenderData::unmapBuffer()
{
	//Do not unmap, if the buffer is not mapped
	if (this->mappedBuffer == nullptr)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	mappedBuffer = nullptr;
	bufferOffset = 0;
}

