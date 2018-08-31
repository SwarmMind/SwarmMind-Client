#include <renderer/TextureRenderData.h>
#include <cstring>
#include <iostream>
#include <imgui/imgui.h>
#include <string>

using namespace std;

TextureRenderData::TextureRenderData()
{
	floatsPerVert = 5;
	unsigned int stride = floatsPerVert * sizeof(GLfloat);
	
	//First Buffer
	vertexBufferSize = 512 * 1024;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Second buffer
	glGenVertexArrays(1, &secondVAO);
	glBindVertexArray(secondVAO);

	glGenBuffers(1, &secondBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, secondBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	mappedBuffer = nullptr;
	bufferOffset = 0;
}

TextureRenderData::~TextureRenderData()
{
	this->unmapBuffer();
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &secondBuffer);
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteVertexArrays(1, &secondVAO);
}

bool TextureRenderData::addData(unsigned int size, const GLfloat* data)
{
	if (!bufferIsMapped())
	{
		mapBuffer();
	}

	if (vertexBufferSize - bufferOffset < size)
	{
		return false;
	}
	else
	{
        if (bufferIsMapped()) {
            memcpy(mappedBuffer + bufferOffset, data, size * sizeof(GLfloat));
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, bufferOffset * sizeof(GLfloat), size * sizeof(GLfloat), data);
        }

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
	swap(vertexArrayObject, secondVAO);
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
	if (!bufferIsMapped())
		return;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	mappedBuffer = nullptr;
	bufferOffset = 0;
}

void TextureRenderData::draw()
{
	int vertexCount = bufferOffset / 5; //Do this before unmapping, so the bufferoffset is not reset
	this->unmapBuffer();

	glBindVertexArray(vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

bool TextureRenderData::bufferIsMapped()
{
	return this->mappedBuffer != nullptr;
}

