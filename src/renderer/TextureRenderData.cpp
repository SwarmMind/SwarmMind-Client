#include <renderer/TextureRenderData.h>
#include <cstring>
#include <iostream>
#include <imgui/imgui.h>
#include <string>

using namespace std;

TextureRenderData::TextureRenderData()
{
	m_floatsPerVert = 5;
	unsigned int stride = m_floatsPerVert * sizeof(GLfloat);
	
	//First Buffer
	m_vertexBufferSize = 512 * 1024;
	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexBufferSize * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//Second buffer
	glGenVertexArrays(1, &m_secondVAO);
	glBindVertexArray(m_secondVAO);

	glGenBuffers(1, &m_secondBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_secondBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexBufferSize * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);


	m_mappedBuffer = nullptr;
	m_bufferOffset = 0;
}

TextureRenderData::~TextureRenderData()
{
	this->unmapBuffer();
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_secondBuffer);
	glDeleteVertexArrays(1, &m_vertexArrayObject);
	glDeleteVertexArrays(1, &m_secondVAO);
}

bool TextureRenderData::addData(unsigned int size, const GLfloat* data)
{
	if (!bufferIsMapped())
	{
		mapBuffer();
	}

	if (m_vertexBufferSize - m_bufferOffset < size)
	{
		return false;
	}
	else
	{
        if (bufferIsMapped()) {
            memcpy(m_mappedBuffer + m_bufferOffset, data, size * sizeof(GLfloat));
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, m_bufferOffset * sizeof(GLfloat), size * sizeof(GLfloat), data);
        }

		m_bufferOffset += size;
		return true;
	}
}

void TextureRenderData::swapBuffers()
{
	//Optimization
	//This will keep the same buffer from getting remapped immediately after being unmapped and used for drawing
	//Therefore glMapBuffer will not block because it will map the buffer currently not used for drawing
	swap(m_vertexBuffer, m_secondBuffer);
	swap(m_vertexArrayObject, m_secondVAO);
}

void TextureRenderData::mapBuffer()
{
	swapBuffers();
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	m_mappedBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	m_bufferOffset = 0;
}

void TextureRenderData::unmapBuffer()
{
	//Do not unmap, if the buffer is not mapped
	if (!bufferIsMapped())
		return;

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	m_mappedBuffer = nullptr;
	m_bufferOffset = 0;
}

void TextureRenderData::draw()
{
	int vertexCount = m_bufferOffset / 5; //Do this before unmapping, so the bufferoffset is not reset
	this->unmapBuffer();

	glBindVertexArray(m_vertexArrayObject);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

bool TextureRenderData::bufferIsMapped()
{
	return this->m_mappedBuffer != nullptr;
}

